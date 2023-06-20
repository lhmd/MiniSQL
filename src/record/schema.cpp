#include "record/schema.h"

/**
 * TODO: Student Implement
 */
uint32_t Schema::SerializeTo(char *buf) const {
  uint32_t ofs = 0;
  MACH_WRITE_INT32(buf + ofs, SCHEMA_MAGIC_NUM);
  ofs += sizeof(uint32_t);
  MACH_WRITE_INT32(buf + ofs, columns_.size());
  ofs += sizeof(uint32_t);
  for (auto column : columns_) {
    ofs += column->SerializeTo(buf + ofs);
  }
  return ofs;
}

uint32_t Schema::GetSerializedSize() const {
  uint32_t size = sizeof(uint32_t) << 1;
  for (auto column : columns_) {
    size += column->GetSerializedSize();
  }
  return size;
}

uint32_t Schema::DeserializeFrom(char *buf, Schema *&schema) {
  uint32_t ofs = 0, magic = MACH_READ_UINT32(buf);
  ofs += sizeof(uint32_t);
  ASSERT(magic == SCHEMA_MAGIC_NUM, "Invalid schema magic number.");
  uint32_t size = MACH_READ_UINT32(buf + ofs);
  ofs += sizeof(uint32_t);
  std::vector<Column *> columns;
  for (uint32_t i = 0; i < size; ++i) {
    Column *temp = nullptr;
    ofs += Column::DeserializeFrom(buf + ofs, temp);
    columns.emplace_back(temp);
  }
  schema = new Schema(columns);
  return ofs;
}