#include "catalog/catalog.h"

void CatalogMeta::SerializeTo(char *buf) const {
    ASSERT(GetSerializedSize() <= PAGE_SIZE, "Failed to serialize catalog metadata to disk.");
    MACH_WRITE_UINT32(buf, CATALOG_METADATA_MAGIC_NUM);
    buf += 4;
    MACH_WRITE_UINT32(buf, table_meta_pages_.size());
    buf += 4;
    MACH_WRITE_UINT32(buf, index_meta_pages_.size());
    buf += 4;
    for (auto iter : table_meta_pages_) {
        MACH_WRITE_TO(table_id_t, buf, iter.first);
        buf += 4;
        MACH_WRITE_TO(page_id_t, buf, iter.second);
        buf += 4;
    }
    for (auto iter : index_meta_pages_) {
        MACH_WRITE_TO(index_id_t, buf, iter.first);
        buf += 4;
        MACH_WRITE_TO(page_id_t, buf, iter.second);
        buf += 4;
    }
}

CatalogMeta *CatalogMeta::DeserializeFrom(char *buf) {
    // check valid
    uint32_t magic_num = MACH_READ_UINT32(buf);
    buf += 4;
    ASSERT(magic_num == CATALOG_METADATA_MAGIC_NUM, "Failed to deserialize catalog metadata from disk.");
    // get table and index nums
    uint32_t table_nums = MACH_READ_UINT32(buf);
    buf += 4;
    uint32_t index_nums = MACH_READ_UINT32(buf);
    buf += 4;
    // create metadata and read value
    CatalogMeta *meta = new CatalogMeta();
    for (uint32_t i = 0; i < table_nums; i++) {
        auto table_id = MACH_READ_FROM(table_id_t, buf);
        buf += 4;
        auto table_heap_page_id = MACH_READ_FROM(page_id_t, buf);
        buf += 4;
        meta->table_meta_pages_.emplace(table_id, table_heap_page_id);
    }
    for (uint32_t i = 0; i < index_nums; i++) {
        auto index_id = MACH_READ_FROM(index_id_t, buf);
        buf += 4;
        auto index_page_id = MACH_READ_FROM(page_id_t, buf);
        buf += 4;
        meta->index_meta_pages_.emplace(index_id, index_page_id);
    }
    return meta;
}

/**
 * TODO: Student Implement
 */
uint32_t CatalogMeta::GetSerializedSize() const {
	return 12 + ((table_meta_pages_.size() + index_meta_pages_.size()) >> 3);
}

CatalogMeta::CatalogMeta() {}

/**
 * TODO: Student Implement
 */
CatalogManager::CatalogManager(BufferPoolManager *buffer_pool_manager, LockManager *lock_manager,
                               LogManager *log_manager, bool init)
    : buffer_pool_manager_(buffer_pool_manager), lock_manager_(lock_manager), log_manager_(log_manager) {
	std::atomic_init(&next_table_id_, 0);
	std::atomic_init(&next_index_id_, 0);
	if (init) {
		// create catalog meta page
		catalog_meta_ = CatalogMeta::NewInstance();
	} else {
		// load catalog meta page
		auto meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
		catalog_meta_ = CatalogMeta::DeserializeFrom(meta_page->GetData());
		// load table meta pages
		for (auto iter : catalog_meta_->table_meta_pages_) {
			auto table_page = buffer_pool_manager_->FetchPage(iter.second);
			TableMetadata *table_meta = nullptr;
			TableMetadata::DeserializeFrom(table_page->GetData(), table_meta);
			auto table_heap = TableHeap::Create(
					buffer_pool_manager_,
					table_meta->GetFirstPageId(),
					table_meta->GetSchema(),
					log_manager_,
					lock_manager_);
			TableInfo *table_info = TableInfo::Create();
			table_info->Init(table_meta, table_heap);
			table_names_.emplace(table_meta->GetTableName(), table_meta->GetTableId());
			tables_.emplace(table_meta->GetTableId(), table_info);

		}
		// load index meta pages
		for (auto iter : catalog_meta_->index_meta_pages_) {
			auto index_page = buffer_pool_manager_->FetchPage(iter.second);
			IndexMetadata *index_meta = nullptr;
			IndexMetadata::DeserializeFrom(index_page->GetData(), index_meta);
			IndexInfo *index_info = IndexInfo::Create();
			index_info->Init(index_meta, tables_[index_meta->GetTableId()], buffer_pool_manager_);
			index_names_[tables_[index_meta->GetTableId()]->GetTableName()][index_meta->GetIndexName()] = index_meta->GetIndexId();
			indexes_[index_meta->GetIndexId()] = index_info;
		}
	}
}

CatalogManager::~CatalogManager() {
  FlushCatalogMetaPage();
  delete catalog_meta_;
  for (auto iter : tables_) {
    delete iter.second;
  }
  for (auto iter : indexes_) {
    delete iter.second;
  }
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::CreateTable(const string &table_name, TableSchema *schema,
                                    Transaction *txn, TableInfo *&table_info) {
	if(table_names_.find(table_name) != table_names_.end()) return DB_TABLE_ALREADY_EXIST;

//	更新name和meta
	table_id_t table_id = catalog_meta_->GetNextTableId();
	table_names_[table_name] = table_id;
	page_id_t page_id;
	auto table_page = buffer_pool_manager_->NewPage(page_id);
	catalog_meta_->table_meta_pages_[table_id] = page_id;

//	创建table_info和heap
	Schema *tmp_schema = Schema::DeepCopySchema(schema);
	auto table_heap = TableHeap::Create(buffer_pool_manager_, tmp_schema, txn, log_manager_, lock_manager_);
	auto table_meta = TableMetadata::Create(table_id, table_name, page_id, tmp_schema);
	table_meta->SerializeTo(table_page->GetData());
	buffer_pool_manager_->UnpinPage(page_id, true);

	table_info = TableInfo::Create();
	table_info->Init(table_meta, table_heap);
	tables_[table_id] = table_info;

//	更新catalog_meta_page
//	auto meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
//	catalog_meta_->SerializeTo(meta_page->GetData());
//	buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);

	return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::GetTable(const string &table_name, TableInfo *&table_info) {
	if(table_names_.find(table_name) == table_names_.end()) return DB_TABLE_NOT_EXIST;
	table_info = tables_[table_names_[table_name]];
	return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::GetTables(vector<TableInfo *> &tables) const {
	if(tables_.empty()) return DB_TABLE_NOT_EXIST;
	for(auto iter : tables_) {
		tables.push_back(iter.second);
	}
	return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::CreateIndex(const std::string &table_name, const string &index_name,
                                    const std::vector<std::string> &index_keys, Transaction *txn,
                                    IndexInfo *&index_info, const string &index_type) {
//	检查table是否不存在，index是否存在
	if(table_names_.find(table_name) == table_names_.end()) return DB_TABLE_NOT_EXIST;
	if(index_names_.find(table_name) != index_names_.end() && index_names_[table_name].find(index_name) != index_names_[table_name].end()) return DB_INDEX_ALREADY_EXIST;

//	检查keys
	auto table_info = tables_[table_names_[table_name]];
	auto table_schema = table_info->GetSchema();
	std::vector<uint32_t> key_map;
	for(auto key : index_keys) {
		index_id_t index_id;
		if(table_schema->GetColumnIndex(key, index_id) == DB_COLUMN_NAME_NOT_EXIST) return DB_COLUMN_NAME_NOT_EXIST;
		key_map.push_back(index_id);
	}

//	更新name和meta
	index_id_t index_id = catalog_meta_->GetNextIndexId();
	index_names_[table_name][index_name] = index_id;
	page_id_t page_id;
	auto index_page = buffer_pool_manager_->NewPage(page_id);
	catalog_meta_->index_meta_pages_[index_id] = page_id;
	buffer_pool_manager_->UnpinPage(page_id, true);
	auto index_meta = IndexMetadata::Create(index_id, index_name, table_names_[table_name], key_map);
	index_meta->SerializeTo(index_page->GetData());

//	创建index_info
	index_info = IndexInfo::Create();
	index_info->Init(index_meta, table_info, buffer_pool_manager_);
	indexes_[index_id] = index_info;

//	更新catalog_meta_page
//	auto meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
//	catalog_meta_->SerializeTo(meta_page->GetData());
//	buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);

	return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::GetIndex(const std::string &table_name, const std::string &index_name,
                                 IndexInfo *&index_info) const {
	if(table_names_.find(table_name) == table_names_.end()) return DB_TABLE_NOT_EXIST;
	if(index_names_.find(table_name) == index_names_.end() || index_names_.at(table_name).find(index_name) == index_names_.at(table_name).end()) return DB_INDEX_NOT_FOUND;
	index_info = indexes_.at(index_names_.at(table_name).at(index_name));
	return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::GetTableIndexes(const std::string &table_name, std::vector<IndexInfo *> &indexes) const {
	if(table_names_.find(table_name) == table_names_.end()) return DB_TABLE_NOT_EXIST;
	for(auto iter : index_names_.at(table_name)) {
		indexes.push_back(indexes_.at(iter.second));
	}
	return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::DropTable(const string &table_name) {
	if(table_names_.find(table_name) == table_names_.end()) return DB_TABLE_NOT_EXIST;
	auto table_id = table_names_[table_name];
	table_names_.erase(table_name);
	catalog_meta_->table_meta_pages_.erase(table_id);
	buffer_pool_manager_->DeletePage(catalog_meta_->table_meta_pages_[table_id]);
	tables_.erase(table_id);
	return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::DropIndex(const string &table_name, const string &index_name) {
	if(table_names_.find(table_name) == table_names_.end()) return DB_TABLE_NOT_EXIST;
	if(index_names_.find(table_name) == index_names_.end() || index_names_.at(table_name).find(index_name) == index_names_.at(table_name).end()) return DB_INDEX_NOT_FOUND;
	auto index_id = index_names_[table_name][index_name];
	index_names_[table_name].erase(index_name);
	catalog_meta_->index_meta_pages_.erase(index_id);
	buffer_pool_manager_->DeletePage(catalog_meta_->index_meta_pages_[index_id]);
	indexes_.erase(index_id);
	return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::FlushCatalogMetaPage() const {
	auto meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
	catalog_meta_->SerializeTo(meta_page->GetData());
	if(buffer_pool_manager_->FlushPage(CATALOG_META_PAGE_ID)) return DB_SUCCESS;
	return DB_FAILED;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::LoadTable(const table_id_t table_id, const page_id_t page_id) {
	if(tables_.find(table_id) != tables_.end()) return DB_TABLE_ALREADY_EXIST;

//	从page中反序列化出table_meta
	auto table_page = buffer_pool_manager_->FetchPage(page_id);
	TableMetadata *table_meta = nullptr;
	TableMetadata::DeserializeFrom(table_page->GetData(), table_meta);
	table_names_[table_meta->GetTableName()] = table_id;

//	创建table_info
	auto table_heap = TableHeap::Create(
			buffer_pool_manager_,
			page_id,
			table_meta->GetSchema(),
			log_manager_,
			lock_manager_);
	auto table_info = TableInfo::Create();
	table_info->Init(table_meta, table_heap);
	tables_[table_id] = table_info;

//	更新catalog_meta_page
//	catalog_meta_->table_meta_pages_[table_id] = page_id;
//	auto meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
//	catalog_meta_->SerializeTo(meta_page->GetData());
//	buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);

	return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::LoadIndex(const index_id_t index_id, const page_id_t page_id) {
	if(indexes_.find(index_id) != indexes_.end()) return DB_INDEX_ALREADY_EXIST;

//	从page中反序列化出index_meta
	auto index_page = buffer_pool_manager_->FetchPage(page_id);
	IndexMetadata *index_meta;
	IndexMetadata::DeserializeFrom(index_page->GetData(), index_meta);
	index_names_[tables_[index_meta->GetTableId()]->GetTableName()][index_meta->GetIndexName()] = index_id;

//	创建index_info
	auto index_info = IndexInfo::Create();
	index_info->Init(index_meta, tables_[index_meta->GetTableId()], buffer_pool_manager_);
	indexes_[index_id] = index_info;

//	更新catalog_meta_page
	catalog_meta_->index_meta_pages_[index_id] = page_id;
	auto meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
	catalog_meta_->SerializeTo(meta_page->GetData());
	buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);

	return DB_SUCCESS;

}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::GetTable(const table_id_t table_id, TableInfo *&table_info) {
	if(tables_.find(table_id) == tables_.end()) return DB_TABLE_NOT_EXIST;
	table_info = tables_[table_id];
	return DB_SUCCESS;
}