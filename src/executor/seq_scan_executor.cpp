//
// Created by njz on 2023/1/17.
//
#include "executor/executors/seq_scan_executor.h"

/**
* TODO: Student Implement
*/
SeqScanExecutor::SeqScanExecutor(ExecuteContext *exec_ctx, const SeqScanPlanNode *plan)
    : AbstractExecutor(exec_ctx),
      plan_(plan){}

void SeqScanExecutor::Init() {
    TableInfo *info = nullptr;
    GetExecutorContext()->GetCatalog()->GetTable(plan_->GetTableName(),info);
    it_ = info->GetTableHeap()->Begin(GetExecutorContext()->GetTransaction());
    end_ = info->GetTableHeap()->End();

    exec_ctx_->GetCatalog()->GetTable(plan_->table_name_, table_info_);
}

bool SeqScanExecutor::Next(Row *row, RowId *rid) {
    while( it_ != end_ ){
        if (  plan_->filter_predicate_ == nullptr ||
                    Field(TypeId::kTypeInt,1).CompareEquals(plan_->filter_predicate_->Evaluate(&(*it_))) ){

            vector<Field> output;output.clear();
            for( auto col : plan_->OutputSchema()->GetColumns() ){
                ::uint32_t col_index;
                table_info_->GetSchema()->GetColumnIndex(col->GetName(),col_index);
                output.push_back(*(*it_).GetField(col_index));
            }

            *row = Row(output);
            row->SetRowId((*it_).GetRowId());
            it_++;
            return true;
        }
        else it_++;
    }
    return false;
}
