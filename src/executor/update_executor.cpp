//
// Created by njz on 2023/1/30.
//

#include "executor/executors/update_executor.h"

UpdateExecutor::UpdateExecutor(ExecuteContext *exec_ctx, const UpdatePlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
        : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {
    exec_ctx_->GetCatalog()->GetTableIndexes(plan_->GetTableName(), index_info_);
}

/**
* TODO: Student Implement
*/
void UpdateExecutor::Init() {
    child_executor_->Init();
}

bool UpdateExecutor::Next([[maybe_unused]] Row *row, RowId *rid) {
    if (child_executor_->Next(row, rid) ) {
        Row old_row = *row;
        RowId old_row_id = *rid;
        *row = GenerateUpdatedTuple(*row);
        TableInfo * table_info;
        exec_ctx_->GetCatalog()->GetTable(plan_->GetTableName(), table_info);

        // Update table.
        table_info->GetTableHeap()->UpdateTuple(*row, *rid, exec_ctx_->GetTransaction());

        *rid = row->GetRowId();

        // Update index.
        for (auto index_info : index_info_) {
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

            // Make it empty.
            while(fields.size() != 0) {
                fields.pop_back();
            }
            for (int i = 0; i < index_info->GetIndexKeySchema()->GetColumnCount(); ++i) {
                fields.push_back(*row->GetField(i));
            }
            Row row_idx(fields);
            index->InsertEntry(
                row_idx,
                *rid,
                exec_ctx_->GetTransaction()
            );
        }

        return true;
    } else {
        return false;
    }
}

Row UpdateExecutor::GenerateUpdatedTuple(const Row &src_row) {
    vector<Field> fields;
    for (int i = 0; i < src_row.GetFieldCount(); ++i) {
        fields.push_back(*src_row.GetField(i));
    }
    auto attrs = plan_->GetUpdateAttr();
    for (auto attr : attrs) {
        fields[attr.first] = *new Field(attr.second->Evaluate(&src_row));
    }
    return Row(fields);
}
