#include "record/row.h"

/**
 * TODO: Student Implement
 */
uint32_t Row::SerializeTo(char *buf, Schema *schema) const {
	ASSERT(schema != nullptr, "Invalid schema before serialize.");
	ASSERT(schema->GetColumnCount() == fields_.size(), "Fields size != column size.");

	uint32_t ofs = 0, size = GetFieldCount();
	MACH_WRITE_INT32(buf + ofs, size);
	ofs += sizeof(uint32_t);
	for(uint32_t i = 0; i < size; ++i) {
		if(fields_[i]->IsNull()) {
			MACH_WRITE_TO(bool, buf + ofs, 0);
			ofs += sizeof(bool);
		} else {
			MACH_WRITE_TO(bool, buf + ofs, 1);
			ofs += sizeof(bool);
		}
	}

	for(uint32_t i = 0; i < size; ++i) {
		if(!fields_[i]->IsNull()) {
			fields_[i]->SerializeTo(buf + ofs);
			ofs += fields_[i]->GetSerializedSize();
		}
	}
	return ofs;
}

uint32_t Row::DeserializeFrom(char *buf, Schema *schema) {
	ASSERT(schema != nullptr, "Invalid schema before serialize.");
	ASSERT(fields_.empty(), "Non empty field in row.");

	uint32_t ofs = 0, size = MACH_READ_UINT32(buf);
	ofs += sizeof(uint32_t);
	bool bitmap[size];
	for(uint32_t i = 0; i < size; ++i) {
		bitmap[i] = MACH_READ_FROM(bool, buf + ofs);
		ofs += sizeof(bool);
	}
	for(uint32_t i = 0; i < size; ++i) {
		if(bitmap[i] == 0) {
			Field *temp=nullptr;
			fields_.emplace_back(temp);
		} else {
			Field *temp=nullptr;
			ofs += Field::DeserializeFrom(buf + ofs, schema->GetColumn(i)->GetType(), &temp, false);
			fields_.emplace_back(temp);
		}
	}
	return ofs;
}

uint32_t Row::GetSerializedSize(Schema *schema) const {
  ASSERT(schema != nullptr, "Invalid schema before serialize.");
  ASSERT(schema->GetColumnCount() == fields_.size(), "Fields size do not match schema's column size.");

  uint32_t size = sizeof(uint32_t);
  for (auto field : fields_) {
	size += sizeof(bool);
	if (!field->IsNull()) {
	  size += field->GetSerializedSize();
	}
  }
  return size;
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
