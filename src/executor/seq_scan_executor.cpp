//
// Created by njz on 2023/1/17.
//
#include "executor/executors/seq_scan_executor.h"

/**
* TODO: Student Implement
*/
SeqScanExecutor::SeqScanExecutor(ExecuteContext *exec_ctx, const SeqScanPlanNode *plan)
    : AbstractExecutor(exec_ctx),
      plan_(plan){
	exec_ctx_->GetCatalog()->GetTable(plan_->GetTableName(), table_info_);
	table_iter_ = table_info_->GetTableHeap()->Begin(exec_ctx_->GetTransaction());
}

void SeqScanExecutor::Init() {
}

bool SeqScanExecutor::Next(Row *row, RowId *rid) {
	while (table_iter_ != table_info_->GetTableHeap()->End()) {
		*row = *table_iter_.operator->();
		*rid = table_iter_->GetRowId();
		++table_iter_;
		if (plan_->GetPredicate() == nullptr) {
			return true;
		}
		if (Field(kTypeInt, kTrue).CompareEquals(plan_->GetPredicate()->Evaluate(row)) == kTrue) {
			return true;
		}
	}
	return false;
}
