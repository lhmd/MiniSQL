//
// Created by Sinon on 2023/6/21.
//
#include "buffer/clock_replacer.h"

CLOCKReplacer::CLOCKReplacer(size_t num_pages) {
  pair clock_item = {true, false};
  for (size_t i = 0; i != num_pages; ++i) {
    clock_status.emplace_back(clock_item);
  }
  clock_hand = 0;
  capacity = 0;
}
CLOCKReplacer::~CLOCKReplacer() = default;

bool CLOCKReplacer::Victim(frame_id_t *frame_id) {
  while (capacity > 0) {
    if (clock_hand == clock_status.size()) {
      clock_hand = 0;
    }

    if (clock_status[clock_hand].first) {
      clock_hand++;
    } else if (clock_status[clock_hand].second) {
      clock_status[clock_hand].second = false;
      clock_hand++;
    } else {
      clock_status[clock_hand].first = true;
      capacity--;
      *frame_id = clock_hand++;
      return true;
    }
  }

  return false;
}

void CLOCKReplacer::Pin(frame_id_t frame_id) {
  if (!clock_status[frame_id].first) {
    clock_status[frame_id].first = true;
    capacity--;
  }
}

void CLOCKReplacer::Unpin(frame_id_t frame_id) {
  if (clock_status[frame_id].first) {
    clock_status[frame_id].first = false;
    clock_status[frame_id].second = true;
    ++capacity;
  }
}

size_t CLOCKReplacer::Size() { return capacity; }