#include "record/row.h"

/**
 * TODO: Student Implement
 */
uint32_t Row::SerializeTo(char *buf, Schema *schema) const {
  ASSERT(schema != nullptr, "Invalid schema before serialize.");
  ASSERT(schema->GetColumnCount() == fields_.size(), "Fields size != column size.");

  uint32_t result = 0;
  MACH_WRITE_TO(uint32_t, buf, ROW_MAGIC_NUM);
  buf += sizeof(uint32_t);
  result += sizeof(uint32_t);
  unsigned int bitmap = 0;
  uint32_t i = 0;
  for (auto iter = fields_.begin(); iter != fields_.end(); ++iter) {
    if ((*iter)->IsNull()) {
      bitmap |= (1 << i);
    } else {
      bitmap &= ~(1 << i);
    }
    i++;
  }
  MACH_WRITE_TO(unsigned int, buf, bitmap);
  buf += sizeof(unsigned int);
  result += sizeof(unsigned int);
  for (auto iter = fields_.begin(); iter != fields_.end(); ++iter) {
    uint32_t num = (*iter)->SerializeTo(buf);
    buf += num;
    result += num;
  }
  return result;
}

uint32_t Row::DeserializeFrom(char *buf, Schema *schema) {
  ASSERT(schema != nullptr, "Invalid schema before serialize.");
  ASSERT(fields_.empty(), "Non empty field in row.");

  uint32_t result = 0;
  uint32_t magic_num = MACH_READ_FROM(uint32_t, buf);
  buf += sizeof(uint32_t);
  result += sizeof(uint32_t);
  if (magic_num != ROW_MAGIC_NUM) {
    return 0;
  }
  uint32_t size = schema->GetColumnCount();
  fields_.resize(size);
  unsigned int bitmap = MACH_READ_FROM(unsigned int, buf);
  buf += sizeof(unsigned int);
  result += sizeof(unsigned int);
  std::vector<Column *> columns = schema->GetColumns();
  for (uint32_t i = 0; i < size; i++) {
    uint32_t num = Field::DeserializeFrom(buf, columns[i]->GetType(), &fields_[i], ((bitmap >> i) & 1) == 1);
    buf += num;
    result += num;
  }
  return result;
}

uint32_t Row::GetSerializedSize(Schema *schema) const {
  ASSERT(schema != nullptr, "Invalid schema before serialize.");
  ASSERT(schema->GetColumnCount() == fields_.size(), "Fields size do not match schema's column size.");

  uint32_t result = 0;
  result += sizeof(uint32_t);
  result += sizeof(unsigned int);
  for (auto iter = fields_.begin(); iter != fields_.end(); ++iter) {
    uint32_t num = (*iter)->GetSerializedSize();
    result += num;
  }
  return result;
}

void Row::GetKeyFromRow(const Schema *schema, const Schema *key_schema, Row &key_row) {
  auto columns = key_schema->GetColumns();
  std::vector<Field> fields;
  uint32_t idx;
  for (auto column : columns) {
    schema->GetColumnIndex(column->GetName(), idx);
    fields.emplace_back(*this->GetField(idx));
  }
  key_row = Row(fields);
}
