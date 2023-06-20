#include "executor/executors/index_scan_executor.h"
#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <vector>
#include "planner/expressions/constant_value_expression.h"
/**
 * TODO: Student Implement
 */
IndexScanExecutor::IndexScanExecutor(ExecuteContext *exec_ctx, const IndexScanPlanNode *plan)
    : AbstractExecutor(exec_ctx), plan_(plan), rec(0) {}

void IndexScanExecutor::MapIt(const AbstractExpressionRef &filter_predicate_, map<uint32_t, pair<string, Field>> &m0) {
  if (filter_predicate_->GetType() == ExpressionType::ComparisonExpression &&
      filter_predicate_->GetChildAt(0)->GetType() == ExpressionType::ColumnExpression &&
      filter_predicate_->GetChildAt(1)->GetType() == ExpressionType::ConstantExpression) {
    auto column_value_expr = dynamic_pointer_cast<ColumnValueExpression>(filter_predicate_->GetChildAt(0));
    auto constant_value_expr = dynamic_pointer_cast<ConstantValueExpression>(filter_predicate_->GetChildAt(1));
    auto comparison_expr = dynamic_pointer_cast<ComparisonExpression>(filter_predicate_);
    uint32_t col_idx = (*&column_value_expr)->GetColIdx();
    pair<string, Field> pair_(comparison_expr->GetComparisonType(), constant_value_expr->val_);
    m0.insert(map<uint32_t, pair<string, Field>>::value_type(col_idx, pair_));
  } else {
    MapIt(filter_predicate_->GetChildAt(0), m0);
    MapIt(filter_predicate_->GetChildAt(1), m0);
  }
}
void IndexScanExecutor::CreateMap(const vector<IndexInfo *> &indexes, map<uint32_t, uint32_t> &m1) {
  // search through all indexes which are included in the condition
  const int max_column_size = 1000;
  int has_index[max_column_size];
  for (int i = 0; i < max_column_size; ++i) has_index[i] = 0;
  for (uint32_t i = 0; i < indexes.size(); ++i) {
    // prepare the value
    string index_name_ = indexes[i]->GetIndexName();
    uint32_t col_idx = (indexes[i]->GetIndexKeySchema()->GetColumns())[0]->GetTableInd();
    if (has_index[col_idx] == 0) {
      has_index[col_idx] = 1;
      m1.insert(map<uint32_t, uint32_t>::value_type(i, col_idx));
    }
  }
}

void IndexScanExecutor::Init() {
  map<uint32_t, pair<string, Field>> m0;
  MapIt(plan_->GetPredicate(), m0);
  map<uint32_t, uint32_t> m1;
  CreateMap(plan_->indexes_, m1);
  // scan index with respective condition
  auto itr = m1.begin();
  vector<Field> key_field;
  key_field.push_back((m0.at(itr->second)).second);
  (plan_->indexes_)[0]->GetIndex()->ScanKey(Row(key_field), res0, nullptr, (m0.at(itr->second)).first);
  for (++itr; itr != m1.end(); ++itr) {
    vector<RowId> res;
    vector<Field> key_field_;
    key_field_.push_back((m0.at(itr->second)).second);
    (plan_->indexes_)[itr->first]->GetIndex()->ScanKey(Row(key_field_), res, nullptr, (m0.at(itr->second)).first);
    sort(res.begin(), res.end(), greater<>());
    auto iter = set_intersection(res0.begin(), res0.end(), res.begin(), res.end(), res0.begin());
    res0.resize(iter - res0.begin());
  }
}

bool IndexScanExecutor::Next(Row *row, RowId *rid) {
  if (rec + 1 > res0.size())
    return false;
  else {
    TableInfo *table_info_;
    GetExecutorContext()->GetCatalog()->GetTable(plan_->GetTableName(), table_info_);
    // wrap rid into row and fetch the tuple from table heap
    row->SetRowId(res0[rec]);
    row->GetFields().clear();
    table_info_->GetTableHeap()->GetTuple(row, nullptr);  // txn = nullptr
    *rid = res0[rec];
    rec++;
  }
  return true;
}
