#include "buffer/lru_replacer.h"

LRUReplacer::LRUReplacer(size_t num_pages){
	this->num_pages = num_pages;
}

LRUReplacer::~LRUReplacer() = default;

/**
 * TODO: Student Implement
 */
bool LRUReplacer::Victim(frame_id_t *frame_id) {
	if(lru_list.empty()) return false;
	frame_id_t first_id = lru_list.front();
	lru_list.pop_front();
	auto it_first = lru_map.find(first_id);
	if(it_first != lru_map.end()) {
		lru_map.erase(it_first);
		*frame_id = first_id;
		return true;
	}
	return false;
}

/**
 * TODO: Student Implement
 */
void LRUReplacer::Pin(frame_id_t frame_id) {
	auto iter = lru_map.find(frame_id);
	if(iter != lru_map.end()) {
		lru_list.erase(iter->second);
		lru_map.erase(iter);
	}
}

/**
 * TODO: Student Implement
 */
void LRUReplacer::Unpin(frame_id_t frame_id) {
	if(lru_map.size() >= num_pages) return;
	auto iter = lru_map.find(frame_id);
	if(iter == lru_map.end()) {
		lru_list.push_back(frame_id);
		lru_map[frame_id] = --lru_list.end();
	}
}

/**
 * TODO: Student Implement
 */
size_t LRUReplacer::Size() {
  return lru_map.size();
}