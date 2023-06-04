#include "storage/table_iterator.h"

#include "common/macros.h"
#include "storage/table_heap.h"

/**
 * TODO: Student Implement
 */
TableIterator::TableIterator(TableHeap *table_heap, RowId rid, Transaction *txn) {
	this->table_heap_ = table_heap;
	this->row_->SetRowId(rid);
	this->txn_ = txn;
	if(rid.GetPageId() != INVALID_PAGE_ID) {
		this->table_heap_->GetTuple(row_, txn_);
	}
}

TableIterator::TableIterator(const TableIterator &other) {
	this->table_heap_ = other.table_heap_;
	this->row_->SetRowId(other.row_->GetRowId());
	this->txn_ = other.txn_;
}

TableIterator::~TableIterator() {}

bool TableIterator::operator==(const TableIterator &itr) const {
	return row_->GetRowId().Get() == itr.row_->GetRowId().Get();
}

bool TableIterator::operator!=(const TableIterator &itr) const {
	return row_->GetRowId().Get() != itr.row_->GetRowId().Get();
}

const Row &TableIterator::operator*() {
	ASSERT(*this != table_heap_->End(), "TableHeap iterator out of range, invalid dereference.");
	return *row_;
}

Row *TableIterator::operator->() {
	ASSERT(*this != table_heap_->End(), "TableHeap iterator out of range, invalid dereference.");
	return row_;
}

TableIterator &TableIterator::operator=(const TableIterator &itr) noexcept {
	this->table_heap_ = itr.table_heap_;
	this->row_ = itr.row_;
	this->txn_ = itr.txn_;
	return *this;
}

// ++iter
TableIterator &TableIterator::operator++() {
	BufferPoolManager *buffer_pool_manager = table_heap_->buffer_pool_manager_;
	auto cur_page = reinterpret_cast<TablePage *>(buffer_pool_manager->FetchPage(row_->GetRowId().GetPageId()));
	cur_page->RLatch();
	assert(cur_page != nullptr);  // all pages are pinned

	RowId next_tuple_rid;
	if (!cur_page->GetNextTupleRid(row_->GetRowId(),
								   &next_tuple_rid)) {  // end of this page
		while (cur_page->GetNextPageId() != INVALID_PAGE_ID) {
			auto next_page = reinterpret_cast<TablePage *>(buffer_pool_manager->FetchPage(cur_page->GetNextPageId()));
			cur_page->RUnlatch();
			buffer_pool_manager->UnpinPage(cur_page->GetTablePageId(), false);
			cur_page = next_page;
			cur_page->RLatch();
			if (cur_page->GetFirstTupleRid(&next_tuple_rid)) {
				break;
			}
		}
	}
	row_ = new Row(next_tuple_rid);

	if (*this != table_heap_->End()) {
		table_heap_->GetTuple(row_ ,nullptr);
	}
	// release until copy the tuple
	cur_page->RUnlatch();
	buffer_pool_manager->UnpinPage(cur_page->GetTablePageId(), false);
	return *this;
}

// iter++
TableIterator TableIterator::operator++(int) {
	TableIterator itr(*this);
	++(*this);
	return itr;
}
