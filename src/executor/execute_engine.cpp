#include "executor/execute_engine.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <chrono>

#include "common/result_writer.h"
#include "executor/executors/delete_executor.h"
#include "executor/executors/index_scan_executor.h"
#include "executor/executors/insert_executor.h"
#include "executor/executors/seq_scan_executor.h"
#include "executor/executors/update_executor.h"
#include "executor/executors/values_executor.h"
#include "glog/logging.h"
#include "planner/planner.h"
#include "utils/utils.h"

extern "C" {
int yyparse(void);
#include "parser/minisql_lex.h"
#include "parser/parser.h"
}

ExecuteEngine::ExecuteEngine(bool is_testing) : is_testing_(is_testing) {
  char path[] = "./databases";
  DIR *dir;
  if ((dir = opendir(path)) == nullptr) {
    mkdir("./databases", 0777);
    dir = opendir(path);
  }
  /** When you have completed all the code for the test,
   *  run it using `main.cpp` and uncomment this part of the code.
   **/
  struct dirent *stdir;
  while ((stdir = readdir(dir)) != nullptr) {
    if (strcmp(stdir->d_name, ".") == 0 || strcmp(stdir->d_name, "..") == 0 || stdir->d_name[0] == '.') continue;
    dbs_[stdir->d_name] = new DBStorageEngine(stdir->d_name, false);
  }
  closedir(dir);
}

std::unique_ptr<AbstractExecutor> ExecuteEngine::CreateExecutor(ExecuteContext *exec_ctx,
                                                                const AbstractPlanNodeRef &plan) {
  switch (plan->GetType()) {
    // Create a new sequential scan executor
    case PlanType::SeqScan: {
      return std::make_unique<SeqScanExecutor>(exec_ctx, dynamic_cast<const SeqScanPlanNode *>(plan.get()));
    }
    // Create a new index scan executor
    case PlanType::IndexScan: {
      return std::make_unique<IndexScanExecutor>(exec_ctx, dynamic_cast<const IndexScanPlanNode *>(plan.get()));
    }
    // Create a new update executor
    case PlanType::Update: {
      auto update_plan = dynamic_cast<const UpdatePlanNode *>(plan.get());
      auto child_executor = CreateExecutor(exec_ctx, update_plan->GetChildPlan());
      return std::make_unique<UpdateExecutor>(exec_ctx, update_plan, std::move(child_executor));
    }
    // Create a new delete executor
    case PlanType::Delete: {
      auto delete_plan = dynamic_cast<const DeletePlanNode *>(plan.get());
      auto child_executor = CreateExecutor(exec_ctx, delete_plan->GetChildPlan());
      return std::make_unique<DeleteExecutor>(exec_ctx, delete_plan, std::move(child_executor));
    }
    case PlanType::Insert: {
      auto insert_plan = dynamic_cast<const InsertPlanNode *>(plan.get());
      auto child_executor = CreateExecutor(exec_ctx, insert_plan->GetChildPlan());
      unique_ptr<InsertExecutor> ret;
      try {
        ret = std::make_unique<InsertExecutor>(exec_ctx, insert_plan, std::move(child_executor));
      } catch (const char *msg) {
        LOG(ERROR) << msg << "\n";
      }
      return ret;
    }
    case PlanType::Values: {
      return std::make_unique<ValuesExecutor>(exec_ctx, dynamic_cast<const ValuesPlanNode *>(plan.get()));
    }
    default:
      throw std::logic_error("Unsupported plan type.");
  }
}

dberr_t ExecuteEngine::ExecutePlan(const AbstractPlanNodeRef &plan, std::vector<Row> *result_set, Transaction *txn,
                                   ExecuteContext *exec_ctx) {
  // Construct the executor for the abstract plan node
  auto executor = CreateExecutor(exec_ctx, plan);
  try {
    executor->Init();
    RowId rid{};
    Row row{};
    while (executor->Next(&row, &rid)) {
      if (result_set != nullptr) {
        result_set->push_back(row);
      }
    }
  } catch (const exception &ex) {
    std::cout << "Error Encountered in Executor Execution: " << ex.what() << std::endl;
    if (result_set != nullptr) {
      result_set->clear();
    }
    return DB_FAILED;
  }
  return DB_SUCCESS;
}

dberr_t ExecuteEngine::Execute(pSyntaxNode ast) {
  if (ast == nullptr) {
    return DB_FAILED;
  }
  auto start_time = std::chrono::system_clock::now();
  unique_ptr<ExecuteContext> context(nullptr);
  if (!current_db_.empty()) context = dbs_[current_db_]->MakeExecuteContext(nullptr);
  switch (ast->type_) {
    case kNodeCreateDB:
      return ExecuteCreateDatabase(ast, context.get());
    case kNodeDropDB:
      return ExecuteDropDatabase(ast, context.get());
    case kNodeShowDB:
      return ExecuteShowDatabases(ast, context.get());
    case kNodeUseDB:
      return ExecuteUseDatabase(ast, context.get());
    case kNodeShowTables:
      return ExecuteShowTables(ast, context.get());
    case kNodeCreateTable:
      return ExecuteCreateTable(ast, context.get());
    case kNodeDropTable:
      return ExecuteDropTable(ast, context.get());
    case kNodeShowIndexes:
      return ExecuteShowIndexes(ast, context.get());
    case kNodeCreateIndex:
      return ExecuteCreateIndex(ast, context.get());
    case kNodeDropIndex:
      return ExecuteDropIndex(ast, context.get());
    case kNodeTrxBegin:
      return ExecuteTrxBegin(ast, context.get());
    case kNodeTrxCommit:
      return ExecuteTrxCommit(ast, context.get());
    case kNodeTrxRollback:
      return ExecuteTrxRollback(ast, context.get());
    case kNodeExecFile:
      return ExecuteExecfile(ast, context.get());
    case kNodeQuit:
      return ExecuteQuit(ast, context.get());
    default:
      break;
  }

  if (current_db_.empty()) {
    LOG(ERROR) << "No database selected.\n";
    return DB_NOT_EXIST;
  }
  // Plan the query.
  Planner planner(context.get());
  std::vector<Row> result_set{};
  try {
    planner.PlanQuery(ast);
    // Execute the query.
    ExecutePlan(planner.plan_, &result_set, nullptr, context.get());
  } catch (const exception &ex) {
    std::cout << "Error Encountered in Planner: " << ex.what() << std::endl;
    return DB_FAILED;
  }
  auto stop_time = std::chrono::system_clock::now();
  double duration_time =
      double((std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time)).count());
  // Although it might not being executing file now, we still count the time.
  exec_file_duration_time_cnt_ += duration_time;
  // Return the result set as string.
  std::stringstream ss;
  ResultWriter writer(ss);

  if (planner.plan_->GetType() == PlanType::SeqScan || planner.plan_->GetType() == PlanType::IndexScan) {
    auto schema = planner.plan_->OutputSchema();
    auto num_of_columns = schema->GetColumnCount();
    if (!result_set.empty()) {
      // find the max width for each column
      vector<int> data_width(num_of_columns, 0);
      for (const auto &row : result_set) {
        for (uint32_t i = 0; i < num_of_columns; i++) {
          data_width[i] = max(data_width[i], int(row.GetField(i)->toString().size()));
        }
      }
      int k = 0;
      for (const auto &column : schema->GetColumns()) {
        data_width[k] = max(data_width[k], int(column->GetName().length()));
        k++;
      }
      // Generate header for the result set.
      writer.Divider(data_width);
      k = 0;
      writer.BeginRow();
      for (const auto &column : schema->GetColumns()) {
        writer.WriteHeaderCell(column->GetName(), data_width[k++]);
      }
      writer.EndRow();
      writer.Divider(data_width);

      // Transforming result set into strings.
      for (const auto &row : result_set) {
        writer.BeginRow();
        for (uint32_t i = 0; i < schema->GetColumnCount(); i++) {
          writer.WriteCell(row.GetField(i)->toString(), data_width[i]);
        }
        writer.EndRow();
      }
      writer.Divider(data_width);
    }
    writer.EndInformation(result_set.size(), duration_time, true);
  } else {
    writer.EndInformation(result_set.size(), duration_time, false);
  }

  if (exec_file_cmds_iter_ == exec_file_cmds_.end() && is_exec_file_) {
    is_exec_file_ = false;
    LOG(INFO) << "Execfile finished, totally cost " << exec_file_duration_time_cnt_ << " ms.";
  }

  std::cout << writer.stream_.rdbuf();
  return DB_SUCCESS;
}

void ExecuteEngine::ExecuteInformation(dberr_t result) {
  switch (result) {
    case DB_ALREADY_EXIST:
      cout << "Database already exists." << endl;
      break;
    case DB_NOT_EXIST:
      cout << "Database not exists." << endl;
      break;
    case DB_TABLE_ALREADY_EXIST:
      cout << "Table already exists." << endl;
      break;
    case DB_TABLE_NOT_EXIST:
      cout << "Table not exists." << endl;
      break;
    case DB_INDEX_ALREADY_EXIST:
      cout << "Index already exists." << endl;
      break;
    case DB_INDEX_NOT_FOUND:
      cout << "Index not exists." << endl;
      break;
    case DB_COLUMN_NAME_NOT_EXIST:
      cout << "Column not exists." << endl;
      break;
    case DB_KEY_NOT_FOUND:
      cout << "Key not exists." << endl;
      break;
    case DB_QUIT:
      cout << "Bye." << endl;
      break;
    case DB_SUCCESS:
      cout << "Success!" << endl;
      break;
    default:
      break;
  }
}

std::string ExecuteEngine::GetNextFileCmd() {
  if (exec_file_cmds_iter_ != exec_file_cmds_.end()) {
    return *exec_file_cmds_iter_++;
  } else {
    return string("");
  }
}

/**
 * TODO: Student Implement DONE
 */
dberr_t ExecuteEngine::ExecuteCreateDatabase(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteCreateDatabase" << std::endl;
#endif
  std::string database_name = ast->child_->val_;
  if (database_name.empty()) {
    return DB_FAILED;
  }
  if (dbs_.find(database_name) != dbs_.end()) {
    return DB_ALREADY_EXIST;
  }
  dbs_[database_name] = new DBStorageEngine(database_name);
  std::cout << "Database " << database_name << " created." << std::endl;
  return DB_SUCCESS;
}

/**
 * TODO: Student Implement DONE
 */
dberr_t ExecuteEngine::ExecuteDropDatabase(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteDropDatabase" << std::endl;
#endif
  string database_name = ast->child_->val_;
  auto it = dbs_.find(database_name);
  if (it == dbs_.end()) {
    return DB_NOT_EXIST;
  }
  string name = "./databases/" + database_name;
  remove(name.c_str());
  dbs_.erase(it);
  std::cout << "Database " << database_name << " dropped." << std::endl;

  // If the current db is the one we are dropping, set the current db to empty.
  if (current_db_ == database_name) {
    current_db_ = "";
  }
  return DB_SUCCESS;
}

/**
 * TODO: Student Implement DONE
 */
dberr_t ExecuteEngine::ExecuteShowDatabases(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteShowDatabases" << std::endl;
#endif
  if (dbs_.empty()) {
    std::cout << "Empty database." << std::endl;
    return DB_SUCCESS;
  }
  std::vector<std::string> databases;
  std::vector<std::vector<std::string>> data;
  for (const auto &db : dbs_) {
    databases.clear();
    databases.push_back(db.first);
    data.push_back(databases);
  }
  std::vector<std::string> Names = {"Database"};
  printDatabaseTable(Names, data);
  return DB_SUCCESS;
}

/**
 * TODO: Student Implement DONE
 */
dberr_t ExecuteEngine::ExecuteUseDatabase(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteUseDatabase" << std::endl;
#endif
  string db_name = ast->child_->val_;
  if (dbs_.find(db_name) == dbs_.end()) {
    return DB_NOT_EXIST;
  }
  current_db_ = db_name;
  std::cout << "Database changed to " << db_name << std::endl;
  return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t ExecuteEngine::ExecuteShowTables(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteShowTables" << std::endl;
#endif
  if (current_db_.empty()) {
    std::cout << "No database selected." << std::endl;
    return DB_FAILED;
  }
  std::vector<std::string> tables;
  std::vector<std::vector<std::string>> data;
  std::vector<TableInfo *> table_infos;
  dbs_[current_db_]->catalog_mgr_->GetTables(table_infos);
  tables.reserve(table_infos.size());
  for (const auto &table_info_ : table_infos) {
    tables.clear();
    tables.emplace_back(table_info_->GetTableName().c_str());
    data.push_back(tables);
  }
  std::vector<std::string> Names = {"Tables_in_" + current_db_};
  printDatabaseTable(Names, data);
  return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t ExecuteEngine::ExecuteCreateTable(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteCreateTable" << std::endl;
#endif
  if (current_db_.empty()) {
    std::cout << "No database selected." << endl;
    return DB_FAILED;
  }

  string table_name(ast->child_->val_);
  auto col_list_node = ast->child_->next_;
  vector<string> primary_key_list, unique_keys, col_names;
  unordered_set<string> primary_key_set;
  vector<TypeId> col_types;
  vector<bool> col_is_unique;
  vector<int> col_manage_len, col_col_id;
  int cnt = 0;
  for (auto it = col_list_node->child_; it != nullptr; it = it->next_) {
    if (it->type_ == kNodeColumnDefinition) {
      col_is_unique.emplace_back(it->val_ != nullptr);
      col_names.emplace_back(it->child_->val_);
      col_col_id.emplace_back(cnt++);
      string col_type_name(it->child_->next_->val_);
      if (col_type_name == "int") {
        col_types.emplace_back(kTypeInt);
        col_manage_len.emplace_back(0);
      } else if (col_type_name == "char") {
        int len = atoi(it->child_->next_->child_->val_);
        if (len < 0) {
          LOG(WARNING) << "Meet invalid char length: " << len;
          return DB_FAILED;
        }
        col_types.emplace_back(kTypeChar);
        col_manage_len.emplace_back(len);
      } else if (col_type_name == "float") {
        col_types.emplace_back(kTypeFloat);
        col_manage_len.emplace_back(0);
      } else {
        LOG(WARNING) << "Meet invalid column type: " << col_type_name;
        return DB_FAILED;
      }
    } else if (it->type_ == kNodeColumnList) {
      // Primary key definition.
      for (auto pk_it = it->child_; pk_it != nullptr; pk_it = pk_it->next_) {
        primary_key_list.emplace_back(pk_it->val_);
        primary_key_set.insert(string(pk_it->val_));
      }
    }
  }

  // Generate the columns vector.
  vector<Column *> columns;
  bool should_manage = false;
  for (int i = 0; i < col_names.size(); ++i) {
    if (primary_key_set.find(col_names[i]) != primary_key_set.end()) {
      if (col_types[i] == kTypeChar) {
        columns.push_back(new Column(col_names[i], col_types[i], col_manage_len[i], i, false, true));
        should_manage = true;
      } else {
        columns.push_back(new Column(col_names[i], col_types[i], i, false, true));
      }
    } else {
      if (col_types[i] == kTypeChar) {
        columns.push_back(new Column(col_names[i], col_types[i], col_manage_len[i], i, false, col_is_unique[i]));
        should_manage = true;
        if (col_is_unique[i]) {
          unique_keys.push_back(col_names[i]);
        }
      } else {
        columns.push_back(new Column(col_names[i], col_types[i], i, false, col_is_unique[i]));
      }
    }
  }

  Schema *schema = new Schema(columns, should_manage);
  TableInfo *table_info;
  dberr_t err = context->GetCatalog()->CreateTable(table_name, schema, context->GetTransaction(), table_info);
  if (err != DB_SUCCESS) {
    return err;
  }

  if (!primary_key_list.empty()) {
    IndexInfo *index_info;
    err = context->GetCatalog()->CreateIndex(table_info->GetTableName(), table_name + "_PK_IDX", primary_key_list,
                                             context->GetTransaction(), index_info, "bptree");
    // Create index on unique.
    for (auto it : unique_keys) {
      string index_name = "UNIQUE_";
      index_name += it + "_";
      index_name += "ON_" + table_name;
      context->GetCatalog()->CreateIndex(table_name, index_name, unique_keys, context->GetTransaction(), index_info,
                                         "btree");
    }
    if (err != DB_SUCCESS) {
      return err;
    }
  }
  return DB_SUCCESS;
}

/**
 * TODO: Student Implement DONE
 */
dberr_t ExecuteEngine::ExecuteDropTable(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteDropTable" << std::endl;
#endif
  if (current_db_.empty()) {
    std::cout << "No database selected." << endl;
    return DB_FAILED;
  }

  // Drop the table.
  string table_name(ast->child_->val_);
  dberr_t err = context->GetCatalog()->DropTable(table_name);
  if (err != DB_SUCCESS) {
    return err;
  }

  // Drop the index.
  std::vector<IndexInfo *> index_info_vec;
  context->GetCatalog()->GetTableIndexes(table_name, index_info_vec);
  for (auto index_info : index_info_vec) {
    err = context->GetCatalog()->DropIndex(table_name, index_info->GetIndexName());
    if (err != DB_SUCCESS) {
      return err;
    }
  }
  cout << "Drop table " << table_name << endl;
  return DB_SUCCESS;
}

/**
 * TODO: Student Implement DONE
 */
dberr_t ExecuteEngine::ExecuteShowIndexes(pSyntaxNode ast, ExecuteContext *context) {
  /**
   * show indexes;
   */
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteShowIndexes" << std::endl;
#endif
  if (current_db_.empty()) {
    std::cout << "No database selected." << endl;
    return DB_FAILED;
  }

  // Get all tables to get indexes.
  std::vector<TableInfo *> table_info_vec;
  context->GetCatalog()->GetTables(table_info_vec);

  // Use map to store the table name and its index info.
  map<string, vector<IndexInfo *>> table_index_vec_pair;
  for (auto table_info : table_info_vec) {
    string table_name = table_info->GetTableName();
    std::vector<IndexInfo *> index_info_vec;
    context->GetCatalog()->GetTableIndexes(table_name, index_info_vec);
    table_index_vec_pair[table_name] = index_info_vec;
  }

  for (auto it : table_index_vec_pair) {
    cout << "@ table \"" << it.first << "\", we have indexes: " << endl;
    for (auto index_info : it.second) {
      cout << "    " << index_info->GetIndexName() << " on columns: ";
      for (auto col : index_info->GetIndexKeySchema()->GetColumns()) {
        cout << " [ " << col->GetName() << " ] ";
      }
      cout << endl;
    }
  }

  return DB_SUCCESS;
}

/**
 * TODO: Student Implement DONE
 */
dberr_t ExecuteEngine::ExecuteCreateIndex(pSyntaxNode ast, ExecuteContext *context) {
  /**
   * create index idx1 on t1(a, b);
   * create index idx1 on t1(a, b) using btree;
   */
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteCreateIndex" << std::endl;
#endif
  // Get the index name, table name, index type and the related columns.
  string index_name(ast->child_->val_);
  string table_name(ast->child_->next_->val_);
  string index_type("bptree");
  vector<string> column_names;
  for (auto it = ast->child_->next_->next_->child_; it != nullptr; it = it->next_) {
    column_names.emplace_back(it->val_);
  }
  // If it defines the index type, load it.
  if (ast->child_->next_->next_->next_ != nullptr) {
    index_type = string(ast->child_->next_->next_->next_->child_->val_);
  }

  // Get table.
  TableInfo *table_info;
  dberr_t err = context->GetCatalog()->GetTable(table_name, table_info);
  if (err != DB_SUCCESS) {
    return err;
  }

  // Create index on the table.
  IndexInfo *index_info;
  err = context->GetCatalog()->CreateIndex(table_name, index_name, column_names, context->GetTransaction(), index_info,
                                           index_type);
  if (err != DB_SUCCESS) {
    return err;
  }

  // Insert old records into the new index.
  auto row_begin = table_info->GetTableHeap()->Begin(context->GetTransaction());
  auto row_end = table_info->GetTableHeap()->End();
  for (auto row = row_begin; row != row_end; ++row) {
    auto rid = (*row).GetRowId();
    // Get related fields.
    vector<Field> fields;
    for (auto col : index_info->GetIndexKeySchema()->GetColumns()) {
      fields.push_back(*(*row).GetField(col->GetTableInd()));
    }
    // The row to be inserted into index.
    Row row_idx(fields);
    err = index_info->GetIndex()->InsertEntry(row_idx, rid, context->GetTransaction());
    if (err != DB_SUCCESS) {
      return err;
    }
  }
  cout << "Create index " << index_name << endl;
  return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t ExecuteEngine::ExecuteDropIndex(pSyntaxNode ast, ExecuteContext *context) {
  /**
   * drop index idx1;
   */
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteDropIndex" << std::endl;
#endif
  // If no database is selected, we should reject the request.
  if (current_db_.empty()) {
    LOG(WARNING) << "No database selected.";
    return DB_FAILED;
  }
  string index_name(ast->child_->val_);

  string table_name;
  vector<TableInfo *> table_info_vec;
  dberr_t err = context->GetCatalog()->GetTables(table_info_vec);
  if (err != DB_SUCCESS) {
    return err;
  }

  for (auto table_info : table_info_vec) {
    IndexInfo *index_info;
    err = context->GetCatalog()->GetIndex(table_info->GetTableName(), index_name, index_info);
    if (err == DB_SUCCESS) {
      table_name = table_info->GetTableName();
      break;
    }
  }

  if (table_name.empty()) {
    std::cout << "No related table found." << endl;
    return DB_INDEX_NOT_FOUND;
  }

  // Drop index.
  context->GetCatalog()->DropIndex(table_name, index_name);

  cout << "Drop index " << index_name << endl;
  return DB_SUCCESS;
}

dberr_t ExecuteEngine::ExecuteTrxBegin(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteTrxBegin" << std::endl;
#endif
  return DB_FAILED;
}

dberr_t ExecuteEngine::ExecuteTrxCommit(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteTrxCommit" << std::endl;
#endif
  return DB_FAILED;
}

dberr_t ExecuteEngine::ExecuteTrxRollback(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteTrxRollback" << std::endl;
#endif
  return DB_FAILED;
}

/**
 * TODO: Student Implement
 */
dberr_t ExecuteEngine::ExecuteExecfile(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteExecfile" << std::endl;
#endif
  string file_name(ast->child_->val_);
  ifstream file(file_name, ios::in);
  // Read in the commands in the file into the vector.
  exec_file_cmds_.resize(0);
  exec_file_duration_time_cnt_ = 0;
  is_exec_file_ = true;
  int cnt = 0;
  char ch;
  const int buf_size = 1024;
  char cmd[buf_size];
  memset(cmd, 0, buf_size);
  if (file.is_open()) {
    while (file.get(ch)) {
      // Judge if a whole command is gotten.
      cmd[cnt++] = ch;
      if (ch == ';') {
        file.get(ch);  // Get the '\n' after ';'.

        exec_file_cmds_.emplace_back(cmd);

        YY_BUFFER_STATE bp = yy_scan_string(cmd);
        yy_switch_to_buffer(bp);
        MinisqlParserInit();
        yyparse();
        auto result = Execute(MinisqlGetParserRootNode());
        MinisqlParserFinish();
        yy_delete_buffer(bp);
        yylex_destroy();
        ExecuteInformation(result);
        if (result == DB_QUIT) {
          break;
        }
        memset(cmd, 0, buf_size);
        cnt = 0;
      }
    }
    file.close();
  }
  exec_file_cmds_iter_ = exec_file_cmds_.begin();

  // Process the commands one by one.
  return DB_SUCCESS;
}

/**
 * TODO: Student Implement DONE
 */
dberr_t ExecuteEngine::ExecuteQuit(pSyntaxNode ast, ExecuteContext *context) {
#ifdef ENABLE_EXECUTE_DEBUG
  LOG(INFO) << "ExecuteQuit" << std::endl;
#endif
  return DB_QUIT;
}

/**
 * My Utils
 */
void ExecuteEngine::printDatabaseTable(const std::vector<std::string> &TableName,
                                       const std::vector<std::vector<std::string>> &vec) {
  // 计算每列的最大宽度
  vector<int> maxNameWidths(TableName.size());
  for (int i = 0; i < TableName.size(); ++i) {
    maxNameWidths[i] = TableName[i].size();
  }
  for (const auto &v : vec) {
    for (int i = 0; i < v.size(); ++i) {
      maxNameWidths[i] = std::max(maxNameWidths[i], (int)v[i].size());
    }
  }

  // 构建表格形式的输出
  std::stringstream output;
  for (int i = 0; i < maxNameWidths.size(); ++i) {
    output << "+";
    for (int j = 0; j < maxNameWidths[i] + 2; ++j) {
      output << "-";
    }
  }
  output << "+\n";

  // 输出表头
  for (int i = 0; i < TableName.size(); ++i) {
    output << "| ";
    output << TableName[i];
    for (int j = 0; j < maxNameWidths[i] - TableName[i].size() + 1; ++j) {
      output << " ";
    }
  }
  output << "|\n";

  output << "+";
  for (int i = 0; i < maxNameWidths.size(); ++i) {
    for (int j = 0; j < maxNameWidths[i] + 2; ++j) {
      output << "-";
    }
    if (i != maxNameWidths.size() - 1) {
      output << "+";
    }
  }
  output << "+\n";

  for (const std::vector<std::string> &v : vec) {
    for (int i = 0; i < v.size(); ++i) {
      output << "| ";
      output << v[i];
      for (int j = 0; j < maxNameWidths[i] - v[i].size() + 1; ++j) {
        output << " ";
      }
    }
    output << "|\n";
  }

  output << "+";
  for (int i = 0; i < maxNameWidths.size(); ++i) {
    for (int j = 0; j < maxNameWidths[i] + 2; ++j) {
      output << "-";
    }
    if (i != maxNameWidths.size() - 1) {
      output << "+";
    }
  }
  output << "+\n";

  // 输出表格
  std::cout << output.str();
}