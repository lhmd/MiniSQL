#include "buffer/buffer_pool_manager.h"

#include "glog/logging.h"
#include "page/bitmap_page.h"

static const char EMPTY_PAGE_DATA[PAGE_SIZE] = {0};

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager) {
  pages_ = new Page[pool_size_];
  replacer_ = new LRUReplacer(pool_size_);
  for (size_t i = 0; i < pool_size_; i++) {
    free_list_.emplace_back(i);
  }
}

BufferPoolManager::~BufferPoolManager() {
  for (auto page : page_table_) {
    FlushPage(page.first);
  }
  delete[] pages_;
  delete replacer_;
}

/**
 * TODO: Student Implement
 */
Page *BufferPoolManager::FetchPage(page_id_t page_id) {
  // 1.     Search the page table for the requested page (P).
  // 1.1    If P exists, pin it and return it immediately.
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  // 2.     If R is dirty, write it back to the disk.
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
  auto page_it = page_table_.find(page_id);
  if (page_it != page_table_.end()) {
    // 1.1    If P exists, pin it and return it immediately.
    frame_id_t frame_id = page_it->second;
    Page *page = &pages_[frame_id];
    replacer_->Pin(frame_id);
    return page;
  } else {
    // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
    frame_id_t frame_id = TryToFindFreePage();
    if (frame_id == INVALID_PAGE_ID && !replacer_->Victim(&frame_id)) {
      return nullptr;
    }
    Page *page = &pages_[frame_id];
    // 2.     If R is dirty, write it back to the disk.
    if (page->IsDirty()) {
      FlushPage(page->GetPageId());
    }
    // 3.     Delete R from the page table and insert P.
    page_table_.erase(page->GetPageId());
    page_table_.insert({page_id, frame_id});
    // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
    page->ResetMemory();
    page->page_id_ = page_id;
    ++page->pin_count_;
    replacer_->Pin(frame_id);
    disk_manager_->ReadPage(page_id, page->GetData());
    return page;
  }
}

/**
 * TODO: Student Implement
 */
Page *BufferPoolManager::NewPage(page_id_t &page_id) {
  // 0.   Make sure you call AllocatePage!
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  // 4.   Set the page ID output parameter. Return a pointer to P.
  size_t flag;
  for (flag = 0; flag < pool_size_; ++flag)
    if (pages_[flag].GetPinCount() == 0) break;
  if (flag == pool_size_) return nullptr;
  frame_id_t frame_id = TryToFindFreePage();
  if (frame_id == INVALID_PAGE_ID && !replacer_->Victim(&frame_id)) return nullptr;
  Page *page = &pages_[frame_id];
  if (page->IsDirty()) FlushPage(page->GetPageId());
  page_id_t new_page_id = AllocatePage();
  page_table_.erase(page->GetPageId());
  page_table_.insert({new_page_id, frame_id});

  page->ResetMemory();
  page->page_id_ = new_page_id;
  ++page->pin_count_;
  replacer_->Pin(frame_id);
  page_id = new_page_id;
  return page;
}

/**
 * TODO: Student Implement
 */
bool BufferPoolManager::DeletePage(page_id_t page_id) {
  // 0.   Make sure you call DeallocatePage!
  // 1.   Search the page table for the requested page (P).
  // 1.   If P does not exist, return true.
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.
  auto page_it = page_table_.find(page_id);
  if (page_it == page_table_.end()) return true;
  frame_id_t frame_id = page_it->second;
  Page *page = &pages_[frame_id];
  if (page->GetPinCount() != 0) return false;
  if (page->IsDirty()) FlushPage(page->GetPageId());
  page_table_.erase(page->GetPageId());
  DeallocatePage(page->GetPageId());

  page->ResetMemory();
  page->page_id_ = INVALID_PAGE_ID;
  page->pin_count_ = 0;
  page->is_dirty_ = false;

  replacer_->Unpin(frame_id);
  return true;
}

/**
 * TODO: Student Implement
 */
bool BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty) {
  auto page_it = page_table_.find(page_id);
  if (page_it == page_table_.end()) return true;
  frame_id_t frame_id = page_it->second;
  Page *page = &pages_[frame_id];
  if (page->GetPinCount() == 0) return false;
  if (is_dirty) page->is_dirty_ = true;
  --page->pin_count_;
  if (page->GetPinCount() == 0) replacer_->Unpin(frame_id);
  return true;
}

/**
 * TODO: Student Implement
 */
bool BufferPoolManager::FlushPage(page_id_t page_id) {
  auto page_it = page_table_.find(page_id);
  if (page_it == page_table_.end()) return false;
  frame_id_t frame_id = page_it->second;
  Page *page = &pages_[frame_id];
  disk_manager_->WritePage(page_id, page->GetData());
  page->is_dirty_ = false;
  return true;
}

page_id_t BufferPoolManager::AllocatePage() {
  int next_page_id = disk_manager_->AllocatePage();
  return next_page_id;
}

void BufferPoolManager::DeallocatePage(__attribute__((unused)) page_id_t page_id) {
  disk_manager_->DeAllocatePage(page_id);
}

bool BufferPoolManager::IsPageFree(page_id_t page_id) { return disk_manager_->IsPageFree(page_id); }

// Only used for debug
bool BufferPoolManager::CheckAllUnpinned() {
  bool res = true;
  for (size_t i = 0; i < pool_size_; i++) {
    if (pages_[i].pin_count_ != 0) {
      res = false;
      LOG(ERROR) << "page " << pages_[i].page_id_ << " pin count:" << pages_[i].pin_count_ << endl;
    }
  }
  return res;
}

frame_id_t BufferPoolManager::TryToFindFreePage() {
  frame_id_t frame_id = INVALID_PAGE_ID;
  if (!free_list_.empty()) {
    frame_id = free_list_.front();
    free_list_.pop_front();
  } else {
    if (!replacer_->Victim(&frame_id)) {
      return INVALID_PAGE_ID;
    }
  }
  return frame_id;
}
