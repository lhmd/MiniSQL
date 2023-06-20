//
// Created by njz on 2023/1/29.
//

#include "executor/executors/delete_executor.h"

/**
* TODO: Student Implement
*/

DeleteExecutor::DeleteExecutor(ExecuteContext *exec_ctx, const DeletePlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
        : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

void DeleteExecutor::Init() {
    child_executor_->Init();
}

bool DeleteExecutor::Next([[maybe_unused]] Row *row, RowId *rid) {
    if (child_executor_->Next(row, rid) ) {
        Row old_row = *row;
        RowId old_row_id = *rid;
        TableInfo * table_info;
        exec_ctx_->GetCatalog()->GetTable(plan_->GetTableName(), table_info);

        // Delete the old record.
        table_info->GetTableHeap()->MarkDelete(*rid, exec_ctx_->GetTransaction());
        table_info->GetTableHeap()->ApplyDelete(*rid, exec_ctx_->GetTransaction());

        // Update index.
        vector<IndexInfo *> index_info_vec;
        exec_ctx_->GetCatalog()->GetTableIndexes(plan_->GetTableName(), index_info_vec);
        for (auto index_info : index_info_vec) {
            auto index = index_info->GetIndex();
            vector<Field> fields;
            for (auto idx : index_info->GetIndexKeySchema()->GetColumns()) {
                string col_name = idx->GetName();
                uint32_t table_idx_id;
                table_info->GetSchema()->GetColumnIndex(col_name, table_idx_id);
                fields.push_back(*old_row.GetField(table_idx_id));
            }
            Row old_row_idx(fields);
            index->RemoveEntry(
                old_row_idx,
                old_row_id,
                exec_ctx_->GetTransaction()
            );
        }

        return true;
    } else {
        return false;
    }
}
