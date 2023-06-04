#include "storage/table_heap.h"

/**
 * TODO: Student Implement
 */
bool TableHeap::InsertTuple(Row &row, Transaction *txn) {
	for(page_id_t id = GetFirstPageId(); id != INVALID_PAGE_ID; ){
		auto page=reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(id));
		if(row.GetSerializedSize(schema_) > page->SIZE_MAX_ROW) return false;
		page->WLatch();
		if(page->InsertTuple(row,schema_,txn,lock_manager_,log_manager_)){
			page->WUnlatch();
			buffer_pool_manager_->UnpinPage(id, true);
			return true;
		}
		page->WUnlatch();
		buffer_pool_manager_->UnpinPage(id,false);
		if(page->GetNextPageId() == INVALID_PAGE_ID){
			page_id_t new_page_id = INVALID_PAGE_ID;
			auto new_page = reinterpret_cast<TablePage *>(buffer_pool_manager_->NewPage(new_page_id));
			page->WLatch();
			new_page->WLatch();
			new_page->Init(new_page_id,id,log_manager_,txn);
			new_page->SetNextPageId(INVALID_PAGE_ID);
			page->SetNextPageId(new_page_id);
			page->WUnlatch();
			new_page->WUnlatch();
			buffer_pool_manager_->UnpinPage(new_page_id,true);
		}
		id=page->GetNextPageId();
	}
	return false;
}

bool TableHeap::MarkDelete(const RowId &rid, Transaction *txn) {
  // Find the page which contains the tuple.
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  // If the page could not be found, then abort the transaction.
  if (page == nullptr) {
    return false;
  }
  // Otherwise, mark the tuple as deleted.
  page->WLatch();
  page->MarkDelete(rid, txn, lock_manager_, log_manager_);
  page->WUnlatch();
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
  return true;
}

/**
 * TODO: Student Implement
 */
bool TableHeap::UpdateTuple(const Row &row, const RowId &rid, Transaction *txn) {
	if(row.GetSerializedSize(schema_) >= PAGE_SIZE) return false;
	// Find the page which contains the tuple.
	auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
	// If the page could not be found, then abort the transaction.
	if(page == nullptr) return false;
	// Otherwise, update the tuple.
	Row old_row;
	page->WLatch();
	TablePage::result_of_update ret = page->TablePage::UpdateTuple(row, &old_row, schema_, txn, lock_manager_, log_manager_);
	if(ret == TablePage::result_of_update::success) {
		MarkDelete(rid, txn);
		InsertTuple(old_row, txn);
		page->WUnlatch();
		buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
		return true;
	}
	else if(ret == TablePage::result_of_update::invalid_slot_num) {
		page->WUnlatch();
		buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
		return false;
	}
	else if(ret == TablePage::result_of_update::not_enough_space) {
		page->WUnlatch();
		buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
		return false;
	}
}

/**
 * TODO: Student Implement
 */
void TableHeap::ApplyDelete(const RowId &rid, Transaction *txn) {
	// Step1: Find the page which contains the tuple.
	// Step2: Delete the tuple from the page.
	auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
	page->WLatch();
	page->ApplyDelete(rid, txn, log_manager_);
	page->WUnlatch();
	buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
}

void TableHeap::RollbackDelete(const RowId &rid, Transaction *txn) {
  // Find the page which contains the tuple.
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  assert(page != nullptr);
  // Rollback to delete.
  page->WLatch();
  page->RollbackDelete(rid, txn, log_manager_);
  page->WUnlatch();
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
}

/**
 * TODO: Student Implement
 */
bool TableHeap::GetTuple(Row *row, Transaction *txn) {
	auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(row->GetRowId().GetPageId()));
//	page->RLatch();
	bool ret = page->GetTuple(row, schema_, txn, lock_manager_);
//	page->RUnlatch();
	buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
	return ret;
}

void TableHeap::DeleteTable(page_id_t page_id) {
  if (page_id != INVALID_PAGE_ID) {
    auto temp_table_page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(page_id));  // 删除table_heap
    if (temp_table_page->GetNextPageId() != INVALID_PAGE_ID)
      DeleteTable(temp_table_page->GetNextPageId());
    buffer_pool_manager_->UnpinPage(page_id, false);
    buffer_pool_manager_->DeletePage(page_id);
  } else {
    DeleteTable(first_page_id_);
  }
}

/**
 * TODO: Student Implement
 */
TableIterator TableHeap::Begin(Transaction *txn) {
	RowId rid(first_page_id_, 0);
	page_id_t page_id = first_page_id_;
	while(page_id != INVALID_PAGE_ID) {
		auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(page_id));
		page->RLatch();
		if(page->GetFirstTupleRid(&rid)) {
			page->RUnlatch();
			buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
			return TableIterator(this, rid, txn);
		}
		page->RUnlatch();
		buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
		page_id = page->GetNextPageId();
	}
	return TableIterator(this, rid, txn);
}

/**
 * TODO: Student Implement
 */
TableIterator TableHeap::End() {
	RowId rid(INVALID_PAGE_ID, 0);
	return TableIterator(this, rid, nullptr);
}
