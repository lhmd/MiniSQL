#include "record/column.h"

#include "glog/logging.h"

Column::Column(std::string column_name, TypeId type, uint32_t index, bool nullable, bool unique)
    : name_(std::move(column_name)), type_(type), table_ind_(index), nullable_(nullable), unique_(unique) {
  ASSERT(type != TypeId::kTypeChar, "Wrong constructor for CHAR type.");
  switch (type) {
    case TypeId::kTypeInt:
      len_ = sizeof(int32_t);
      break;
    case TypeId::kTypeFloat:
      len_ = sizeof(float_t);
      break;
    default:
      ASSERT(false, "Unsupported column type.");
  }
}

Column::Column(std::string column_name, TypeId type, uint32_t length, uint32_t index, bool nullable, bool unique)
    : name_(std::move(column_name)),
      type_(type),
      len_(length),
      table_ind_(index),
      nullable_(nullable),
      unique_(unique) {
  ASSERT(type == TypeId::kTypeChar, "Wrong constructor for non-VARCHAR type.");
}

Column::Column(const Column *other)
    : name_(other->name_),
      type_(other->type_),
      len_(other->len_),
      table_ind_(other->table_ind_),
      nullable_(other->nullable_),
      unique_(other->unique_) {}

/**
* TODO: Student Implement
*/
uint32_t Column::SerializeTo(char *buf) const {
	uint32_t ofs = 0;
	/* serialize magic number */
	MACH_WRITE_UINT32(buf + ofs, COLUMN_MAGIC_NUM);
	ofs += sizeof(uint32_t);
	/* serialize column name */
	MACH_WRITE_UINT32(buf + ofs, name_.size());
	ofs += sizeof(uint32_t);
	memcpy(buf + ofs, name_.c_str(), name_.size());
	ofs += name_.size();
	/* serialize column type */
	MACH_WRITE_TO(TypeId, buf + ofs, type_);
	ofs += sizeof(TypeId);
	/* serialize column length */
	MACH_WRITE_UINT32(buf + ofs, len_);
	ofs += sizeof(uint32_t);
	/* serialize column index */
	MACH_WRITE_UINT32(buf + ofs, table_ind_);
	ofs += sizeof(uint32_t);
	/* serialize column nullable */
	MACH_WRITE_TO(bool, buf + ofs, nullable_);
	ofs += sizeof(bool);
	/* serialize column unique */
	MACH_WRITE_TO(bool, buf + ofs, unique_);
	ofs += sizeof(bool);

	return ofs;
}

/**
 * TODO: Student Implement
 */
uint32_t Column::GetSerializedSize() const {
	return (sizeof(uint32_t) << 2) + name_.size() + sizeof(TypeId) + (sizeof(bool) << 1);
}

/**
 * TODO: Student Implement
 */
uint32_t Column::DeserializeFrom(char *buf, Column *&column) {
	if (column != nullptr) {
		LOG(WARNING) << "Pointer to column is not null in column deserialize." 									 << std::endl;
	}

	/* deserialize field from buf */
	uint32_t magic_num = MACH_READ_UINT32(buf);
	uint32_t ofs = sizeof(uint32_t);
	if (magic_num != COLUMN_MAGIC_NUM) {
		LOG(WARNING) << "Magic number does not match in column deserialize." 									 << std::endl;
	}
	/* deserialize column name */
	uint32_t col_name_len = MACH_READ_UINT32(buf + ofs);
	ofs += sizeof(uint32_t);
	std::string column_name(buf + ofs, col_name_len);
	ofs += col_name_len;
	/* deserialize column type */
	TypeId type = MACH_READ_FROM(TypeId, buf + ofs);
	ofs += sizeof(TypeId);
	/* deserialize column length */
	uint32_t col_len = MACH_READ_UINT32(buf + ofs);
	ofs += sizeof(uint32_t);
	/* deserialize column index */
	uint32_t col_ind = MACH_READ_UINT32(buf + ofs);
	ofs += sizeof(uint32_t);
	/* deserialize column nullable */
	bool nullable = MACH_READ_FROM(bool, buf + ofs);
	ofs += sizeof(bool);
	/* deserialize column unique */
	bool unique = MACH_READ_FROM(bool, buf + ofs);
	ofs += sizeof(bool);

	/* allocate object */
	if (type == kTypeChar) {
		column = new Column(column_name, type, col_len, col_ind, nullable, unique);
	} else {
		column = new Column(column_name, type, col_ind, nullable, unique);
	}
	return ofs;
}
