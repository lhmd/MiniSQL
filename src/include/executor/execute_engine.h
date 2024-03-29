#ifndef MINISQL_EXECUTE_ENGINE_H
#define MINISQL_EXECUTE_ENGINE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "common/dberr.h"
#include "common/instance.h"
#include "executor/execute_context.h"
#include "executor/executors/abstract_executor.h"
#include "executor/plans/abstract_plan.h"
#include "record/row.h"
#include "transaction/transaction.h"

extern "C" {
#include "parser/parser.h"
};

/**
 * ExecuteEngine
 */
class ExecuteEngine {
 public:
  ExecuteEngine(bool is_testing = true);

  ~ExecuteEngine() {
    for (auto it : dbs_) {
      delete it.second;
    }
  }

  /**
   * executor interface
   */
  dberr_t Execute(pSyntaxNode ast);

  dberr_t ExecutePlan(const AbstractPlanNodeRef &plan, std::vector<Row> *result_set, Transaction *txn,
                      ExecuteContext *exec_ctx);

  void ExecuteInformation(dberr_t result);

  std::string GetNextFileCmd();

 private:
  static std::unique_ptr<AbstractExecutor> CreateExecutor(ExecuteContext *exec_ctx, const AbstractPlanNodeRef &plan);

  dberr_t ExecuteCreateDatabase(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteDropDatabase(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteShowDatabases(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteUseDatabase(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteShowTables(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteCreateTable(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteDropTable(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteShowIndexes(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteCreateIndex(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteDropIndex(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteTrxBegin(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteTrxCommit(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteTrxRollback(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteExecfile(pSyntaxNode ast, ExecuteContext *context);

  dberr_t ExecuteQuit(pSyntaxNode ast, ExecuteContext *context);

 private:
  std::unordered_map<std::string, DBStorageEngine *> dbs_; /** all opened databases */
  std::string current_db_;                                 /** current database */
  std::vector<std::string> exec_file_cmds_;                /** commands in execfile */
  std::vector<std::string>::iterator exec_file_cmds_iter_; /** iterator of exec_file_cmds_ */
  uint32_t exec_file_duration_time_cnt_;
  bool is_exec_file_;
  bool is_testing_;

  // create by lhmd
  void printDatabaseTable(const vector<std::string> &TableName, const vector<std::vector<std::string>> &vec);
};

#endif  // MINISQL_EXECUTE_ENGINE_H
