#include "storage/disk_manager.h"

#include <sys/stat.h>

#include <filesystem>
#include <stdexcept>

#include "glog/logging.h"
#include "page/bitmap_page.h"

DiskManager::DiskManager(const std::string &db_file) : file_name_(db_file) {
  std::scoped_lock<std::recursive_mutex> lock(db_io_latch_);
  db_io_.open(db_file, std::ios::binary | std::ios::in | std::ios::out);
  // directory or file does not exist
  if (!db_io_.is_open()) {
    db_io_.clear();
    // create a new file
    std::filesystem::path p = db_file;
    if (p.has_parent_path()) std::filesystem::create_directories(p.parent_path());
    db_io_.open(db_file, std::ios::binary | std::ios::trunc | std::ios::out);
    db_io_.close();
    // reopen with original mode
    db_io_.open(db_file, std::ios::binary | std::ios::in | std::ios::out);
    if (!db_io_.is_open()) {
      throw std::exception();
    }
  }
  ReadPhysicalPage(META_PAGE_ID, meta_data_);
}

void DiskManager::Close() {
  std::scoped_lock<std::recursive_mutex> lock(db_io_latch_);
  if (!closed) {
    db_io_.close();
    closed = true;
  }
}

void DiskManager::ReadPage(page_id_t logical_page_id, char *page_data) {
  ASSERT(logical_page_id >= 0, "Invalid page id.");
  ReadPhysicalPage(MapPageId(logical_page_id), page_data);
}

void DiskManager::WritePage(page_id_t logical_page_id, const char *page_data) {
  ASSERT(logical_page_id >= 0, "Invalid page id.");
  WritePhysicalPage(MapPageId(logical_page_id), page_data);
}

/**
 * TODO: Student Implement
 */
page_id_t DiskManager::AllocatePage() {
  DiskFileMetaPage *meta_page = reinterpret_cast<DiskFileMetaPage *>(meta_data_);
  char buffer[PAGE_SIZE] = {};
  uint32_t meta_flag, page_flag = 0;
  for (meta_flag = 0; meta_flag < meta_page->GetExtentNums(); ++meta_flag)
    if (meta_page->extent_used_page_[meta_flag] < BITMAP_SIZE) break;
  if (meta_flag == meta_page->GetExtentNums()) {
    if (meta_flag == (PAGE_SIZE - 8) / 4) return INVALID_PAGE_ID;
    meta_page->extent_used_page_[meta_flag] = 1;
    ++meta_page->num_allocated_pages_;
    ++meta_page->num_extents_;
    BitmapPage<PAGE_SIZE> *bitmap_page = reinterpret_cast<BitmapPage<PAGE_SIZE> *>(buffer);
    bitmap_page->AllocatePage(page_flag);
  } else {
    ReadPhysicalPage(meta_flag * (BITMAP_SIZE + 1) + 1, buffer);
    BitmapPage<PAGE_SIZE> *bitmap_page = reinterpret_cast<BitmapPage<PAGE_SIZE> *>(buffer);
    //		for(page_flag = 0; page_flag < bitmap_page->GetMaxSupportedSize(); ++page_flag)
    //if(bitmap_page->IsPageFree(page_flag)) break;
    ++meta_page->extent_used_page_[meta_flag];
    ++meta_page->num_allocated_pages_;
    bitmap_page->AllocatePage(page_flag);
  }
  WritePhysicalPage(meta_flag * (BITMAP_SIZE + 1) + 1, buffer);
  return meta_page->num_allocated_pages_ - 1;
}

/**
 * TODO: Student Implement
 */
void DiskManager::DeAllocatePage(page_id_t logical_page_id) {
  if (IsPageFree(logical_page_id)) return;
  DiskFileMetaPage *meta_page = reinterpret_cast<DiskFileMetaPage *>(meta_data_);
  char buffer[PAGE_SIZE];
  ReadPhysicalPage(logical_page_id / BITMAP_SIZE * (BITMAP_SIZE + 1) + 1, buffer);
  BitmapPage<PAGE_SIZE> *bitmap_page = reinterpret_cast<BitmapPage<PAGE_SIZE> *>(buffer);

  uint32_t page_id = logical_page_id % BITMAP_SIZE;
  meta_page->num_allocated_pages_--;
  meta_page->extent_used_page_[logical_page_id / BITMAP_SIZE]--;
  bitmap_page->DeAllocatePage(page_id);
  WritePhysicalPage(1 + logical_page_id / BITMAP_SIZE * (BITMAP_SIZE + 1), buffer);
}

/**
 * TODO: Student Implement
 */
bool DiskManager::IsPageFree(page_id_t logical_page_id) {
  char buffer[PAGE_SIZE];
  ReadPhysicalPage(logical_page_id / BITMAP_SIZE * (BITMAP_SIZE + 1) + 1, buffer);
  BitmapPage<PAGE_SIZE> *bitmap_page = reinterpret_cast<BitmapPage<PAGE_SIZE> *>(buffer);
  return bitmap_page->IsPageFree(logical_page_id % BITMAP_SIZE);
}

/**
 * TODO: Student Implement
 */
page_id_t DiskManager::MapPageId(page_id_t logical_page_id) {
  return logical_page_id + logical_page_id / BITMAP_SIZE + 2;
}

int DiskManager::GetFileSize(const std::string &file_name) {
  struct stat stat_buf;
  int rc = stat(file_name.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

void DiskManager::ReadPhysicalPage(page_id_t physical_page_id, char *page_data) {
  int offset = physical_page_id * PAGE_SIZE;
  // check if read beyond file length
  if (offset >= GetFileSize(file_name_)) {
#ifdef ENABLE_BPM_DEBUG
    LOG(INFO) << "Read less than a page" << std::endl;
#endif
    memset(page_data, 0, PAGE_SIZE);
  } else {
    // set read cursor to offset
    db_io_.seekp(offset);
    db_io_.read(page_data, PAGE_SIZE);
    // if file ends before reading PAGE_SIZE
    int read_count = db_io_.gcount();
    if (read_count < PAGE_SIZE) {
#ifdef ENABLE_BPM_DEBUG
      LOG(INFO) << "Read less than a page" << std::endl;
#endif
      memset(page_data + read_count, 0, PAGE_SIZE - read_count);
    }
  }
}

void DiskManager::WritePhysicalPage(page_id_t physical_page_id, const char *page_data) {
  size_t offset = static_cast<size_t>(physical_page_id) * PAGE_SIZE;
  // set write cursor to offset
  db_io_.seekp(offset);
  db_io_.write(page_data, PAGE_SIZE);
  // check for I/O error
  if (db_io_.bad()) {
    LOG(ERROR) << "I/O error while writing";
    return;
  }
  // needs to flush to keep disk file in sync
  db_io_.flush();
}