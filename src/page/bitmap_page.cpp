#include "page/bitmap_page.h"

#include "glog/logging.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
/**
 * TODO: Student Implement
 */
template <size_t PageSize>
bool BitmapPage<PageSize>::AllocatePage(uint32_t &page_offset) {
  if (next_free_page_ == GetMaxSupportedSize() || !IsPageFree(next_free_page_)) {
    return false;
  }
  page_allocated_++;
  page_offset = next_free_page_;
  uint32_t byte_index = next_free_page_ / 8;
  uint8_t bit_index = next_free_page_ % 8;
  bytes[byte_index] ^= (1 << bit_index);
  for(uint32_t i = next_free_page_; i < GetMaxSupportedSize(); i++) {
    if (IsPageFree(i)) {
      next_free_page_ = i;
      break;
    }
  }
  return true;
}

/**
 * TODO: Student Implement
 */
template <size_t PageSize>
bool BitmapPage<PageSize>::DeAllocatePage(uint32_t page_offset) {
  uint32_t byte_index = page_offset / 8;
  uint8_t bit_index = page_offset % 8;
  if(IsPageFreeLow(byte_index, bit_index)) {
    return false;
  }
  --page_allocated_;
  bytes[byte_index] ^= (1 << bit_index);
  if(page_offset < next_free_page_) {
    next_free_page_ = page_offset;
  }
  return true;
}

/**
 * TODO: Student Implement
 */
template <size_t PageSize>
bool BitmapPage<PageSize>::IsPageFree(uint32_t page_offset) const {
  uint32_t byte_index = page_offset / 8;
  uint8_t bit_index = page_offset % 8;
  return IsPageFreeLow(byte_index, bit_index);
}

template <size_t PageSize>
bool BitmapPage<PageSize>::IsPageFreeLow(uint32_t byte_index, uint8_t bit_index) const {
  return (bytes[byte_index] & (1 << bit_index)) == 0;
}

template class BitmapPage<64>;

template class BitmapPage<128>;

template class BitmapPage<256>;

template class BitmapPage<512>;

template class BitmapPage<1024>;

template class BitmapPage<2048>;

template class BitmapPage<4096>;
#pragma clang diagnostic pop