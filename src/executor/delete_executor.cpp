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
    GetExecutorContext()->GetCatalog()->GetTable(plan_->table_name_,table_info_);
    GetExecutorContext()->GetCatalog()->GetTableIndexes(plan_->table_name_,indexes_);
}

bool DeleteExecutor::Next([[maybe_unused]] Row *row, RowId *rid) {
	// delete row first
    Row* delete_row = new Row();
    if( child_executor_->Next(delete_row,nullptr) ){
        if(!table_info_->GetTableHeap()->MarkDelete(delete_row->GetRowId(), nullptr)) return false;
        for( auto index : indexes_ ){
            vector<Field> key_contain;
            for( auto col : index->GetIndexKeySchema()->GetColumns() ){
                uint32_t col_index;
                table_info_->GetSchema()->GetColumnIndex(col->GetName(),col_index);
                key_contain.push_back(*(delete_row->GetField(col_index)));
            }
            index->GetIndex()->RemoveEntry(Row(key_contain),delete_row->GetRowId(), nullptr);
        }
        return true;
    } else
		return false;
}