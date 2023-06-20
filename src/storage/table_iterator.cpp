#include "storage/table_iterator.h"

#include "common/macros.h"
#include "storage/table_heap.h"

/**
 * TODO: Student Implement
 */
TableIterator::TableIterator() {
	table_heap_ = nullptr;
	row_ = Row();
	txn_ = nullptr;
}
TableIterator::TableIterator(TableHeap *table_heap, Row row, Transaction *txn)
: table_heap_(table_heap), row_(row), txn_(txn) {
}

TableIterator::TableIterator(const TableIterator &other) {
	this->table_heap_ = other.table_heap_;
	this->row_ = other.row_;
	this->txn_ = other.txn_;
}

TableIterator::~TableIterator() {}

bool TableIterator::operator==(const TableIterator &itr) const {
    return (table_heap_ == itr.table_heap_) && (row_.GetRowId() == itr.row_.GetRowId());
}

bool TableIterator::operator!=(const TableIterator &itr) const {
    return !(operator==(itr));
}

const Row &TableIterator::operator*() {
    return row_;
}

Row *TableIterator::operator->() {
    return &row_;
}

TableIterator &TableIterator::operator=(const TableIterator &itr) noexcept {
	this->table_heap_ = itr.table_heap_;
	this->row_ = itr.row_;
	this->txn_ = itr.txn_;
	return *this;
}

// ++iter
TableIterator &TableIterator::operator++() {
    if(*this == table_heap_->End()){
        return *this;
    }
    TablePage* page = reinterpret_cast<TablePage* >(table_heap_->buffer_pool_manager_->FetchPage(row_.GetRowId().GetPageId()));
    RowId next_id = RowId();
    page->RLatch();
    if(page->GetNextTupleRid(row_.GetRowId(),&next_id)){
        row_.destroy();
        row_.SetRowId(next_id);
        page->GetTuple(&row_,table_heap_->schema_,txn_,table_heap_->lock_manager_);
        page->RUnlatch();
        table_heap_->buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
        return *this;
    }
    page_id_t next_page_id;
    while((next_page_id = page->GetNextPageId()) != INVALID_PAGE_ID){
        page->RUnlatch();
        table_heap_->buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
        page = reinterpret_cast<TablePage* >(table_heap_->buffer_pool_manager_->FetchPage(next_page_id));
        page->RLatch();
        if(page->GetFirstTupleRid(&next_id)){
            row_.destroy();
            row_.SetRowId(next_id);
            page->GetTuple(&row_,table_heap_->schema_,txn_,table_heap_->lock_manager_);
            page->RUnlatch();
            table_heap_->buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
            return *this;
        }
    }
    page->RUnlatch();
    table_heap_->buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
    *this = table_heap_->End();
    return *this;
}

// iter++
TableIterator TableIterator::operator++(int) {
	TableIterator itr(*this);
	++(*this);
	return itr;
}
