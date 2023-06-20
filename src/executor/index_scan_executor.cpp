#include "executor/executors/index_scan_executor.h"
#include <algorithm>
/**
* TODO: Student Implement DONE
*/

void intersection(vector<RowId> a, vector<RowId> b, vector<RowId> &res) {
    sort(a.begin(), a.end());
    sort(b.begin(), b.end());
    set_intersection(a.begin(), a.end(), b.begin(), b.end(), back_inserter(res));
}

IndexScanExecutor::IndexScanExecutor(ExecuteContext *exec_ctx, const IndexScanPlanNode *plan)
        : AbstractExecutor(exec_ctx), plan_(plan) {
    LOG(INFO) << "IDX SCAN\n";
    // Get the table_info for convenience.
    auto table_name = plan_->GetTableName();
    exec_ctx_->GetCatalog()->GetTable(table_name, table_info_);
    // Get filter_ for convenience.
    filter_ = plan_->GetPredicate();

    // Process the results tuples.
    results_.resize(0);
    vector<AbstractExpression *> comp_exprs;
    filter_->FindAllComparisonExpressions(comp_exprs);
    // It stores the id of rows gotten from each index.
    vector<RowId> single_idx_results;
    for (auto it : plan_->indexes_) {
        // As we don't know what type the comparison can be, we use this to control the type we can handle.
        bool can_handle = false;

        // Get the position of the indexed column in the table.
        column_id_t idx_col_pos = it->GetIndexKeySchema()->GetColumn(0)->GetTableInd();
        // Construct the reference row.
        auto schema = table_info_->GetSchema();
        auto schema_col = schema->GetColumns();

        // The compared bound list.
        vector< vector<Field> > ref_fields_list;
        // The comparison operator string list.
        vector<string> comp_str_list;

        // Find the related comparison expression in all comparison expressions.
        for (auto expr : comp_exprs) {
            auto lhs = expr->GetChildAt(0);
            auto rhs = expr->GetChildAt(1);
            vector<Field> ref_fields;
            string comp_str;

            if (lhs->GetType() == ExpressionType::ColumnExpression && rhs->GetType() == ExpressionType::ConstantExpression) {
                // Like `name = 'Alice'`.
                auto col_exper = dynamic_pointer_cast<ColumnValueExpression>(lhs);
                if (col_exper->GetColIdx() == idx_col_pos) {
                    // Find the related comparison expression.
                    can_handle = true;
                    // Replace the reference row's target field.
                    Field ref_field = rhs->Evaluate(nullptr);
                    ref_fields.push_back(ref_field);
                    comp_str = dynamic_cast<ComparisonExpression *>(expr)->GetComparisonType();

                    ref_fields_list.push_back(ref_fields);
                    comp_str_list.push_back(comp_str);
                }
            } else if (lhs->GetType() == ExpressionType::ConstantExpression && rhs->GetType() == ExpressionType::ColumnExpression) {
                // Like `'Alice' = name`;
                auto col_exper = dynamic_pointer_cast<ColumnValueExpression>(rhs);
                if (col_exper->GetColIdx() == idx_col_pos) {
                    // Find the related comparison expression.
                    can_handle = true;
                    // Replace the reference row's target field.
                    Field ref_field = lhs->Evaluate(nullptr);
                    ref_fields.push_back(ref_field);
                    comp_str = dynamic_cast<ComparisonExpression *>(expr)->GetComparisonType();
                    // Reverse the comparison operator.
                    if (comp_str == "<") {
                      comp_str = ">";
                    } else if (comp_str == ">") {
                      comp_str = "<";
                    } else if (comp_str == "<=") {
                      comp_str = ">=";
                    } else if (comp_str == ">=") {
                      comp_str = "<=";
                    }

                    ref_fields_list.push_back(ref_fields);
                    comp_str_list.push_back(comp_str);
                }
            } else {
                continue;
            }
        }
        if (!can_handle) {
            continue;
        }
        for (int i = 0; i < ref_fields_list.size(); ++i) {
            auto ref_fields = ref_fields_list[i];
            auto comp_str = comp_str_list[i];
            // Construct the reference row to scan key.
            Row ref_row(ref_fields);
            single_idx_results.resize(0);
            // Scan the index.
            it->GetIndex()->ScanKey(ref_row, single_idx_results, exec_ctx_->GetTransaction(), comp_str);
            if (results_.empty()) {
                results_ = single_idx_results;
            } else {
                auto old_results = results_;
                results_.resize(0);
                intersection(old_results, single_idx_results, results_);
            }
        }
    }
}

void IndexScanExecutor::Init() {
    // Reset the table_iterator to the `begin()`.
    results_iter_ = results_.begin();
}

bool IndexScanExecutor::Next(Row *row, RowId *rid) {
    do {
        // If the results_iter_ is at the end, return false.
        if (results_iter_ == results_.end()) {
            return false;
        }
        // Pre self-increase and copy the current iterator to process.
        auto it_cp = results_iter_++;

        // Construct the row_id.
        *rid = *(new RowId(*it_cp));
        // Construct the row with row_id to get the data.
        *row = *(new Row(*rid));
        table_info_->GetTableHeap()->GetTuple(row, exec_ctx_->GetTransaction());
        if (plan_->need_filter_) {
            // If need filter, evaluate the filter.
            if (filter_->Evaluate(row).CompareEquals(Field(kTypeInt, 1))) {
                return true;
            } else {
                continue;
            }
        } else {
            // If not need filter, return true directly.
            return true;
        }
    } while(true);
}
