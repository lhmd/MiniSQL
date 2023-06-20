//
// Created by njz on 2023/1/27.
//
#include "executor/executors/insert_executor.h"

/**
* TODO: Student Implement DONE
*/
InsertExecutor::InsertExecutor(ExecuteContext *exec_ctx, const InsertPlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
        : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {
    // Get table to be inserted.
    string table_name = plan_->GetTableName();
    exec_ctx_->GetCatalog()->GetTable(table_name, table_info_);

    // Get all related index.
    vector<IndexInfo *> related_index_info_vec;
    exec_ctx_->GetCatalog()->GetTableIndexes(table_name, related_index_info_vec);
    
    // Get child's executor and count them.
    influence_total_ = 0;
    child_executor_->Init();
    Row row;
    RowId rid;
    while (child_executor_->Next(&row, &rid)) {
        // Check if the record already exists using all unique index key.
        vector<RowId> rids;
        vector<Row> row_idx_vec;
        bool has_record = false;
        for (auto idx : related_index_info_vec) {
            vector<Field> fields;
            // Get fields to be scanned.
            for (auto col : idx->GetIndexKeySchema()->GetColumns()) {
                fields.push_back(*row.GetField(col->GetTableInd()));
            }
            Row row_idx(fields);
            row_idx_vec.push_back(row_idx);
            dberr_t err = idx->GetIndex()->ScanKey(row_idx, rids, exec_ctx_->GetTransaction());
            has_record = has_record || (err == DB_SUCCESS);
        }
        // If already has this record, we ignore it.
        if (has_record) {
            LOG(WARNING) << "Duplicate record!";
            continue;
        }

        ++influence_total_;
        
        // Update table.
        table_info_->GetTableHeap()->InsertTuple(row, exec_ctx_->GetTransaction());
        rid = row.GetRowId();

        // Update all related indexes.
        for (auto index_info : related_index_info_vec) {
            auto index = index_info->GetIndex();
            vector<Field> fields;
            for (auto idx : index_info->GetIndexKeySchema()->GetColumns()) {
                string col_name = idx->GetName();
                uint32_t table_idx_id;
                table_info_->GetSchema()->GetColumnIndex(col_name, table_idx_id);
                fields.push_back(*row.GetField(table_idx_id));
            }
            Row row_idx(fields);
            index_info->GetIndex()->InsertEntry(
                row_idx, 
                rid, 
                exec_ctx_->GetTransaction()
            );
        }
    }
}

void InsertExecutor::Init() {
    influence_count_ = 0;
}

bool InsertExecutor::Next([[maybe_unused]] Row *row, RowId *rid) {
    // Only return null.
    if (influence_count_ < influence_total_) {
        ++influence_count_;
        return true;
    } else {
        return false;
    }
}
