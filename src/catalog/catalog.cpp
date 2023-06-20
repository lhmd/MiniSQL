#include "catalog/catalog.h"

void CatalogMeta::SerializeTo(char *buf) const {
    uint32_t mask = 0;
    ASSERT(GetSerializedSize() <= PAGE_SIZE, "Failed to serialize catalog metadata to disk.");
    MACH_WRITE_UINT32(buf, CATALOG_METADATA_MAGIC_NUM);
    mask ^= CATALOG_METADATA_MAGIC_NUM;
    buf += 4;
    MACH_WRITE_UINT32(buf, table_meta_pages_.size());
    mask ^= table_meta_pages_.size();
    buf += 4;
    MACH_WRITE_UINT32(buf, index_meta_pages_.size());
    mask ^= index_meta_pages_.size();
    buf += 4;
    for (auto iter : table_meta_pages_) {
        MACH_WRITE_TO(table_id_t, buf, iter.first);
        mask ^= iter.first;
        buf += 4;
        MACH_WRITE_TO(page_id_t, buf, iter.second);
        mask ^= iter.second;
        buf += 4;
    }
    for (auto iter : index_meta_pages_) {
        MACH_WRITE_TO(index_id_t, buf, iter.first);
        mask ^= iter.first;
        buf += 4;
        MACH_WRITE_TO(page_id_t, buf, iter.second);
        mask ^= iter.second;
        buf += 4;
    }
    MACH_WRITE_UINT32(buf, mask);
    buf += 4;
}

CatalogMeta *CatalogMeta::DeserializeFrom(char *buf) {
    uint32_t mask_cal = 0;
    // check valid
    uint32_t magic_num = MACH_READ_UINT32(buf);
    mask_cal ^= magic_num;
    buf += 4;
    ASSERT(magic_num == CATALOG_METADATA_MAGIC_NUM, "Failed to deserialize catalog metadata from disk.");
    // get table and index nums
    uint32_t table_nums = MACH_READ_UINT32(buf);
    mask_cal ^= table_nums;
    buf += 4;
    uint32_t index_nums = MACH_READ_UINT32(buf);
    mask_cal ^= index_nums;
    buf += 4;
    // create metadata and read value
    CatalogMeta *meta = new CatalogMeta();
    for (uint32_t i = 0; i < table_nums; i++) {
        auto table_id = MACH_READ_FROM(table_id_t, buf);
        mask_cal ^= table_id;
        buf += 4;
        auto table_heap_page_id = MACH_READ_FROM(page_id_t, buf);
        mask_cal ^= table_heap_page_id;
        buf += 4;
        meta->table_meta_pages_.emplace(table_id, table_heap_page_id);
    }
    for (uint32_t i = 0; i < index_nums; i++) {
        auto index_id = MACH_READ_FROM(index_id_t, buf);
        mask_cal ^= index_id;
        buf += 4;
        auto index_page_id = MACH_READ_FROM(page_id_t, buf);
        mask_cal ^= index_page_id;
        buf += 4;
        meta->index_meta_pages_.emplace(index_id, index_page_id);
    }
    uint32_t mask_get;
    mask_get = MACH_READ_UINT32(buf);
    buf += 4;
    ASSERT(mask_get == mask_cal, "Failed to deserialize catalog metadata from disk cause getting error mask.");
    return meta;
}

/**
 * TODO: Student Implement
 */
uint32_t CatalogMeta::GetSerializedSize() const {
    uint32_t result = 0;
    result += sizeof(uint32_t) * 3;
    result += sizeof(uint32_t) * 2 * table_meta_pages_.size();
    result += sizeof(uint32_t) * 2 * index_meta_pages_.size();
    return result;
}

CatalogMeta::CatalogMeta() {}

/**
 * TODO: Student Implement
 */
CatalogManager::CatalogManager(BufferPoolManager *buffer_pool_manager, LockManager *lock_manager,
                               LogManager *log_manager, bool init)
    : buffer_pool_manager_(buffer_pool_manager), lock_manager_(lock_manager), log_manager_(log_manager) {
//	std::atomic_init(&next_table_id_, 0);
//	std::atomic_init(&next_index_id_, 0);
	if (init) {
		catalog_meta_ = CatalogMeta::NewInstance();
		FlushCatalogMetaPage();
	} else {
		catalog_meta_ = CatalogMeta::DeserializeFrom(buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID)->GetData());
		// Get tables info.
		for (auto [tid, pid] : catalog_meta_->table_meta_pages_) {
			// Get meta data from buffer pool.
			TableMetadata * table_meta;
			TableMetadata::DeserializeFrom(buffer_pool_manager_->FetchPage(pid)->GetData(), table_meta);
			TableHeap * table_heap = TableHeap::Create(buffer_pool_manager_, table_meta->GetFirstPageId(), table_meta->GetSchema(), log_manager_, lock_manager_);

			// Create table info.
			TableInfo * table_info = TableInfo::Create();
			table_info->Init(table_meta, table_heap);
			// Insert into catalog.
			tables_[tid] = table_info;
			table_names_[table_meta->GetTableName()] = tid;
		}
		// Get indexes info.
		for (auto it : catalog_meta_->index_meta_pages_) {
			// Get meta data from buffer pool.
			index_id_t iid = it.first;
			page_id_t  pid = it.second;
			IndexMetadata * index_meta;
			IndexMetadata::DeserializeFrom(buffer_pool_manager_->FetchPage(pid)->GetData(), index_meta);
			// Create index info.
			table_id_t tid = index_meta->GetTableId();
			TableInfo * table_info = tables_.at(tid);
			ASSERT(table_info != nullptr, "Table info should not be null.");
			IndexInfo * index_info = IndexInfo::Create();

			// INDEX TYPE SET
			// index_info->Init(index_meta, table_info, buffer_pool_manager, "hash");

			index_info->Init(index_meta, table_info, buffer_pool_manager);

			// Insert into catalog.
			indexes_.emplace(iid, index_info);
			string iname = index_meta->GetIndexName();
			string tname = table_info->GetTableName();
			if (index_names_.find(tname) != index_names_.end()) {
				index_names_[tname].emplace(iname, iid);
			} else {
				// If `tname` table has not inserted yet, create a new map.
				index_names_.emplace(tname, unordered_map<string, index_id_t>());
				index_names_[tname].emplace(iname, iid);
			}
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
	auto meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
	catalog_meta_->SerializeTo(meta_page->GetData());
	buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);

	return DB_SUCCESS;
}

/**
 * TODO: Student Implement
 */
dberr_t CatalogManager::GetTable(const string &table_name, TableInfo *&table_info) {
    // Check if the table exists.
    if (table_names_.find(table_name) == table_names_.end()) {
        // Not found.
        return DB_TABLE_NOT_EXIST;
    }
    table_id_t tid = table_names_[table_name];
    // Get table_info with table_id.
    if (tables_.find(tid) == tables_.end()) {
        // Not found.
        LOG(ERROR) << "Error happens when trying to get table_info with existed table_id!";
        return DB_FAILED;
    }
    table_info = tables_[tid];
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
	auto meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
	catalog_meta_->SerializeTo(meta_page->GetData());
	buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);

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
    // Check if the table name exists.
    if (index_names_.find(table_name) == index_names_.end()) {
        return DB_TABLE_NOT_EXIST;
    }
    // Make sure the indexes vector is empty.
    indexes.resize(0);
    // Get all indexes of the table.
    auto & table_indexes_name_ = index_names_.at(table_name);
    for (auto it : table_indexes_name_) {
        index_id_t iid = it.second;
        indexes.push_back(indexes_.at(iid));
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
	tables_[table_id]->GetTableHeap()->FreeTableHeap();
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
	catalog_meta_->table_meta_pages_[table_id] = page_id;
	auto meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
	catalog_meta_->SerializeTo(meta_page->GetData());
	buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);

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
    if (tables_.find(table_id) == tables_.end()) {
        return DB_TABLE_NOT_EXIST;
    }
    table_info = tables_.at(table_id);
    return DB_SUCCESS;
}