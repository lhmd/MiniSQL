//
// Created by njz on 2023/1/17.
//
#include "executor/executors/seq_scan_executor.h"

/**
 * TODO: Student Implement DONE
 */
SeqScanExecutor::SeqScanExecutor(ExecuteContext *exec_ctx, const SeqScanPlanNode *plan)
        : AbstractExecutor(exec_ctx), plan_(plan) {
    // LOG(INFO) << "SeqScanExecutor::SeqScanExecutor";
    // Get the table_info for convenience.
    auto table_name = plan_->GetTableName();
    exec_ctx_->GetCatalog()->GetTable(table_name, table_info_);
    // Get filter_ for convenience.
    filter_ = plan_->GetPredicate();
}

void SeqScanExecutor::Init() {
    table_iter_ = table_info_->GetTableHeap()->Begin(exec_ctx_->GetTransaction());
}

bool SeqScanExecutor::Next(Row *row, RowId *rid) {
    // LOG(INFO) << "SeqScanExecutor::Next";
    do {
        // First check if the iterator meet the end. If it does meet, return false.
        if (table_iter_ == table_info_->GetTableHeap()->End()) {
            return false;
        }
        // Pre self-increase and copy the current iterator to process.
        auto it_cp = table_iter_++;

        // Otherwise, set the return data and continue to iterate the table.

        // Construct a copy of RowId using  copy constructor.
        *rid = *(new RowId(it_cp->GetRowId()));

        // Extract fields and transform it from `vec<Type *>` to `vec<Type>` to construct new `Row` object.
        vector<Field *> field_ptrs = it_cp->GetFields();
        vector<Field> field_cps;
        for (auto it : field_ptrs) {
            field_cps.emplace_back(*it);
        }
        Row row_cp(field_cps);

        // If it has filter, we should evaluate if it fits the condition before return.
        if (filter_!= nullptr) {
            // If not equal, the evaluation fails, we should go on.
            // `Field(kTypeInt, 1)` acts as `true` in this frame.
            if (!filter_->Evaluate(&row_cp).CompareEquals(Field(kTypeInt, 1))) {
                continue;
            }
        }

        // Then return the projected columns.
        while (field_cps.size() != 0) {
            field_cps.pop_back();
        }
        auto schema = plan_->OutputSchema();
        for (auto col : schema->GetColumns()) {
            field_cps.emplace_back(*it_cp->GetField(col->GetTableInd()));
        }
        *row = *(new Row(field_cps));
        row->SetRowId(*rid);
        return true;
    } while(true);
}
