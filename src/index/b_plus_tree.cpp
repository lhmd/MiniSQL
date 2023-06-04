#include "index/b_plus_tree.h"

#include <string>

#include "glog/logging.h"
#include "index/basic_comparator.h"
#include "index/generic_key.h"
#include "page/index_roots_page.h"

/**
 * TODO: Student Implement
 */
BPlusTree::BPlusTree(index_id_t index_id, BufferPoolManager *buffer_pool_manager, const KeyManager &KM,
                     int leaf_max_size, int internal_max_size)
    : index_id_(index_id),
      buffer_pool_manager_(buffer_pool_manager),
      processor_(KM),
      leaf_max_size_(leaf_max_size),
      internal_max_size_(internal_max_size) {
	auto page = reinterpret_cast<IndexRootsPage *>(buffer_pool_manager_->FetchPage(INDEX_ROOTS_PAGE_ID));
	if(!page->GetRootId(index_id_, &root_page_id_)) {
		root_page_id_ = INVALID_PAGE_ID;
	}
	buffer_pool_manager_->UnpinPage(INDEX_ROOTS_PAGE_ID, true);
	buffer_pool_manager_->UnpinPage(root_page_id_, true);
}

void BPlusTree::Destroy(page_id_t current_page_id) {
	if(IsEmpty()) return;
	auto page = buffer_pool_manager_->FetchPage(current_page_id);
	auto node = reinterpret_cast<BPlusTreePage *>(page->GetData());
	if (node->IsLeafPage()) {
		auto *leaf_node = reinterpret_cast<LeafPage *>(page->GetData());
		leaf_node->SetSize(0);
		buffer_pool_manager_->UnpinPage(current_page_id, false);
		buffer_pool_manager_->DeletePage(current_page_id);
		return;
	}
	auto *internal_node = reinterpret_cast<InternalPage *>(page->GetData());
	for (int i = 0; i < internal_node->GetSize(); ++i) {
		Destroy(internal_node->ValueAt(i));
	}
	internal_node->SetSize(0);
	buffer_pool_manager_->UnpinPage(current_page_id, false);
	buffer_pool_manager_->DeletePage(current_page_id);
	if(current_page_id == root_page_id_) {
		root_page_id_ = INVALID_PAGE_ID;
	}
}

/*
 * Helper function to decide whether current b+tree is empty
 */
bool BPlusTree::IsEmpty() const {
	return root_page_id_ == INVALID_PAGE_ID;
}

/*****************************************************************************
 * SEARCH
 *****************************************************************************/
/*
 * Return the only value that associated with input key
 * This method is used for point query
 * @return : true means key exists
 */
bool BPlusTree::GetValue(const GenericKey *key, std::vector<RowId> &result, Transaction *transaction) {
	if(IsEmpty()) return false;
//	auto root_page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(root_page_id_));
	auto page = FindLeafPage(key, root_page_id_);
	auto leaf_node = reinterpret_cast<BPlusTreeLeafPage *>(page->GetData());
	page->RLatch();
	RowId rid;
	if(leaf_node->Lookup(key, rid, processor_)) {
		page->RUnlatch();
		buffer_pool_manager_->UnpinPage(leaf_node->GetPageId(), false);
		result.push_back(rid);
		return true;
	} else {
		page->RUnlatch();
		buffer_pool_manager_->UnpinPage(leaf_node->GetPageId(), false);
		return false;
	}
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/*
 * Insert constant key & value pair into b+ tree
 * if current tree is empty, start new tree, update root page id and insert
 * entry, otherwise insert into leaf page.
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false, otherwise return true.
 */
bool BPlusTree::Insert(GenericKey *key, const RowId &value, Transaction *transaction) {
//	std::ofstream out("output.txt", std::ios_base::app);
//	PrintTree(out);
//	out.close();
	if(IsEmpty()) {
		StartNewTree(key, value);
		return true;
	}
	return InsertIntoLeaf(key, value, transaction);
}
/*
 * Insert constant key & value pair into an empty tree
 * User needs to first ask for new page from buffer pool manager(NOTICE: throw
 * an "out of memory" exception if returned value is nullptr), then update b+
 * tree's root page id and insert entry directly into leaf page.
 */
void BPlusTree::StartNewTree(GenericKey *key, const RowId &value) {
	leaf_max_size_ = (PAGE_SIZE - LEAF_PAGE_HEADER_SIZE) / (processor_.GetKeySize() + sizeof(RowId)) - 1;
	internal_max_size_ = (PAGE_SIZE - INTERNAL_PAGE_HEADER_SIZE) / (processor_.GetKeySize() + sizeof(RowId)) - 1;
	auto page = buffer_pool_manager_->NewPage(root_page_id_);
	auto node = reinterpret_cast<LeafPage *>(page->GetData());
	node->Init(root_page_id_, INVALID_PAGE_ID, processor_.GetKeySize(), leaf_max_size_);
	node->Insert(key, value, processor_);
	UpdateRootPageId(1);
	buffer_pool_manager_->UnpinPage(root_page_id_, true);
}

/*
 * Insert constant key & value pair into leaf page
 * User needs to first find the right leaf page as insertion target, then look
 * through leaf page to see whether insert key exist or not. If exist, return
 * immediately, otherwise insert entry. Remember to deal with split if necessary.
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false, otherwise return true.
 */
bool BPlusTree::InsertIntoLeaf(GenericKey *key, const RowId &value, Transaction *transaction) {
	auto page = FindLeafPage(key, root_page_id_);
	auto leaf_node = reinterpret_cast<LeafPage*>(page->GetData());
//	ToString(leaf_page, buffer_pool_manager_);
	RowId rid;
	if(leaf_node->Lookup(key, rid, processor_)) {
		page->WUnlatch();
		buffer_pool_manager_->UnpinPage(page->GetPageId(), false);
		return false;
	}
	int new_size = leaf_node->Insert(key, value, processor_);
	if(new_size < leaf_max_size_) {
		page->WUnlatch();
		buffer_pool_manager_->UnpinPage(leaf_node->GetPageId(), true);
	} else {
		auto sibling = Split(leaf_node, transaction);
		auto sibling_key = sibling->KeyAt(0);
		InsertIntoParent(leaf_node, sibling_key, sibling, transaction);
		buffer_pool_manager_->UnpinPage(sibling->GetPageId(), true);
	}
	return true;
}

/*
 * Split input page and return newly created page.
 * Using template N to represent either internal page or leaf page.
 * User needs to first ask for new page from buffer pool manager(NOTICE: throw
 * an "out of memory" exception if returned value is nullptr), then move half
 * of key & value pairs from input page to newly created page
 */
BPlusTreeInternalPage *BPlusTree::Split(InternalPage *node, Transaction *transaction) {
	page_id_t new_page_id;
	auto new_page = buffer_pool_manager_->NewPage(new_page_id);
	auto new_node = reinterpret_cast<InternalPage*>(new_page->GetData());
	new_node->SetPageType(IndexPageType::INTERNAL_PAGE);
	new_node->Init(new_page->GetPageId(), node->GetParentPageId(),node->GetKeySize(),internal_max_size_);
	node->MoveHalfTo(new_node, buffer_pool_manager_);
	buffer_pool_manager_->UnpinPage(new_page_id, true);
	buffer_pool_manager_->UnpinPage(node->GetPageId(), true);
	return new_node;
}

BPlusTreeLeafPage *BPlusTree::Split(LeafPage *node, Transaction *transaction) {
//	ToString(node, buffer_pool_manager_);
	page_id_t new_page_id;
	auto new_page = buffer_pool_manager_->NewPage(new_page_id);
	auto new_node = reinterpret_cast<LeafPage*>(new_page->GetData());
	new_node->Init(new_page_id,node->GetParentPageId(),node->GetKeySize(),node->GetMaxSize());
	node->MoveHalfTo(new_node);
	new_node->SetParentPageId(node->GetParentPageId());
	new_node->SetNextPageId(node->GetNextPageId());
	node->SetNextPageId(new_node->GetPageId());
//	ToString(node, buffer_pool_manager_);
//	ToString(new_node, buffer_pool_manager_);
	buffer_pool_manager_->UnpinPage(node->GetPageId(), true);
	buffer_pool_manager_->UnpinPage(new_page_id, true);
	return new_node;
}

/*
 * Insert key & value pair into internal page after split
 * @param   old_node      input page from split() method
 * @param   key
 * @param   new_node      returned page from split() method
 * User needs to first find the parent page of old_node, parent node must be
 * adjusted to take info of new_node into account. Remember to deal with split
 * recursively if necessary.
 */
void BPlusTree::InsertIntoParent(BPlusTreePage *old_node, GenericKey *key, BPlusTreePage *new_node,
                                 Transaction *transaction) {
	if(old_node->IsRootPage()) {
		auto new_root = reinterpret_cast<InternalPage *>(buffer_pool_manager_->NewPage(root_page_id_)->GetData());
		new_root->SetPageType(IndexPageType::INTERNAL_PAGE);
		new_root->Init(root_page_id_, INVALID_PAGE_ID,  processor_.GetKeySize(), internal_max_size_);
		new_root->PopulateNewRoot(old_node->GetPageId(), key, new_node->GetPageId());
		old_node->SetParentPageId(new_root->GetPageId());
		new_node->SetParentPageId(new_root->GetPageId());
		buffer_pool_manager_->UnpinPage(new_root->GetPageId(), true);
		UpdateRootPageId(0);
	} else {
		auto parent_page = buffer_pool_manager_->FetchPage(old_node->GetParentPageId());
		auto parent = reinterpret_cast<InternalPage *>(parent_page->GetData());
//		int old_size = parent->GetSize();
		int new_size = parent->InsertNodeAfter(old_node->GetPageId(), key, new_node->GetPageId());

		if(new_size < internal_max_size_) {
			old_node->SetParentPageId(parent->GetPageId());
			new_node->SetParentPageId(parent->GetPageId());
			parent_page->WUnlatch();
			buffer_pool_manager_->UnpinPage(parent->GetPageId(), true);
		} else {
			auto sibling = Split(parent, transaction);
			for(int i = 0; i < parent->GetSize(); ++i) {
				auto child = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(parent->ValueAt(i))->GetData());
				child->SetParentPageId(parent->GetPageId());
				buffer_pool_manager_->UnpinPage(child->GetPageId(), true);
			}
			for(int i = 0; i < sibling->GetSize(); ++i) {
				auto child = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(sibling->ValueAt(i))->GetData());
				child->SetParentPageId(sibling->GetPageId());
				buffer_pool_manager_->UnpinPage(child->GetPageId(), true);
			}
			auto key = sibling->KeyAt(0);
			InsertIntoParent(parent, key, sibling, transaction);
			parent_page->WUnlatch();
			buffer_pool_manager_->UnpinPage(parent->GetPageId(), true);
			buffer_pool_manager_->UnpinPage(sibling->GetPageId(), true);
		}
	}
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/*
 * Delete key & value pair associated with input key
 * If current tree is empty, return immediately.
 * If not, User needs to first find the right leaf page as deletion target, then
 * delete entry from leaf page. Remember to deal with redistribute or merge if
 * necessary.
 */
void BPlusTree::Remove(const GenericKey *key, Transaction *transaction) {
	if(IsEmpty()) return;
	auto page = FindLeafPage(key, root_page_id_);
	auto leaf_node = reinterpret_cast<LeafPage *>(page->GetData());
	int old_size = leaf_node->GetSize();
	int new_size = leaf_node->RemoveAndDeleteRecord(key, processor_);
	if(old_size == new_size) {
		page->WUnlatch();
		buffer_pool_manager_->UnpinPage(leaf_node->GetPageId(), false);
	} else {
		page->WUnlatch();
		if(!leaf_node->IsRootPage() && new_size < leaf_node->GetMinSize()) {
			CoalesceOrRedistribute(leaf_node, transaction);
		}
		buffer_pool_manager_->UnpinPage(leaf_node->GetPageId(), true);
	}
}

/* todo
 * User needs to first find the sibling of input page. If sibling's size + input
 * page's size > page's max size, then redistribute. Otherwise, merge.
 * Using template N to represent either internal page or leaf page.
 * @return: true means target leaf page should be deleted, false means no
 * deletion happens
 */
template <typename N>
bool BPlusTree::CoalesceOrRedistribute(N *&node, Transaction *transaction) {
	if(IsEmpty()) return false;
	if(node->IsRootPage()) {
		if(node->GetSize() == 1 && AdjustRoot(node)) {
			buffer_pool_manager_->UnpinPage(node->GetPageId(), true);
			buffer_pool_manager_->DeletePage(node->GetPageId());
		}
		return false;
	}
	if(node->GetSize() >= node->GetMinSize()) {
		buffer_pool_manager_->UnpinPage(node->GetPageId(), true);
		return false;
	}

	auto parent = reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(node->GetParentPageId())->GetData());
	int index = parent->ValueIndex(node->GetPageId()), sibling_index;

	bool is_Tail = index == parent->GetSize() - 1, flag;
	sibling_index = is_Tail ? index - 1 : index + 1;
	page_id_t sibling_id = parent->ValueAt(sibling_index);
	auto sibling_page = buffer_pool_manager_->FetchPage(sibling_id);
	auto sibling = reinterpret_cast<N *>(sibling_page->GetData());
	if(sibling->GetSize() + node->GetSize() >= node->GetMaxSize()) {
		if(is_Tail) Redistribute(sibling, node, sibling_index);
		else Redistribute(sibling, node, 0);
		buffer_pool_manager_->UnpinPage(parent->GetPageId(), true);
		buffer_pool_manager_->UnpinPage(sibling_id, true);
		buffer_pool_manager_->UnpinPage(node->GetPageId(), true);
		return false;
	} else {
		if(is_Tail) flag = Coalesce(sibling, node, parent, index, transaction);
		else flag = Coalesce(node, sibling, parent, sibling_index, transaction);
		buffer_pool_manager_->UnpinPage(parent->GetPageId(), true);
		buffer_pool_manager_->UnpinPage(sibling_id, true);
		buffer_pool_manager_->UnpinPage(node->GetPageId(), true);
		return flag;
	}
}

/*
 * Move all the key & value pairs from one page to its sibling page, and notify
 * buffer pool manager to delete this page. Parent page must be adjusted to
 * take info of deletion into account. Remember to deal with coalesce or
 * redistribute recursively if necessary.
 * Using template N to represent either internal page or leaf page.
 * @param   neighbor_node      sibling page of input "node"
 * @param   node               input from method coalesceOrRedistribute()
 * @param   parent             parent page of input "node"
 * @return  true means parent node should be deleted, false means no deletion happened
 */
bool BPlusTree::Coalesce(LeafPage *&neighbor_node, LeafPage *&node, InternalPage *&parent, int index,
                         Transaction *transaction) {
	node->MoveAllTo(neighbor_node);
	buffer_pool_manager_->UnpinPage(node->GetPageId(), true);
	parent->Remove(index);
	if(parent->GetSize() < parent->GetMinSize()) {
		return CoalesceOrRedistribute(parent, transaction);
	} else {
		return false;
	}
}

bool BPlusTree::Coalesce(InternalPage *&neighbor_node, InternalPage *&node, InternalPage *&parent, int index,
                         Transaction *transaction) {
	auto middle_key = parent->KeyAt(index);
	node->MoveAllTo(neighbor_node, middle_key, buffer_pool_manager_);
	buffer_pool_manager_->UnpinPage(neighbor_node->GetPageId(), true);
	for(int i = 0; i < neighbor_node->GetSize(); i++) {
		auto child_page = buffer_pool_manager_->FetchPage(neighbor_node->ValueAt(i));
		auto child = reinterpret_cast<BPlusTreePage *>(child_page->GetData());
		child->SetParentPageId(neighbor_node->GetPageId());
		buffer_pool_manager_->UnpinPage(child->GetPageId(), true);
	}
	parent->Remove(index);
	if(parent->GetSize() < parent->GetMinSize()) {
		return CoalesceOrRedistribute(parent, transaction);
	} else {
		return false;
	}
}

/*
 * Redistribute key & value pairs from one page to its sibling page. If index ==
 * 0, move sibling page's first key & value pair into end of input "node",
 * otherwise move sibling page's last key & value pair into head of input
 * "node".
 * Using template N to represent either internal page or leaf page.
 * @param   neighbor_node      sibling page of input "node"
 * @param   node               input from method coalesceOrRedistribute()
 */
void BPlusTree::Redistribute(LeafPage *neighbor_node, LeafPage *node, int index) {
	if(index == 0) {
		neighbor_node->MoveFirstToEndOf(node);
		if(neighbor_node->GetParentPageId() != INVALID_PAGE_ID) {
			auto parent_page = buffer_pool_manager_->FetchPage(neighbor_node->GetParentPageId());
			auto parent = reinterpret_cast<InternalPage *>(parent_page->GetData());
			parent->SetKeyAt(parent->ValueIndex(neighbor_node->GetPageId()), neighbor_node->KeyAt(0));
			buffer_pool_manager_->UnpinPage(parent->GetPageId(), true);
		}
	} else {
		neighbor_node->MoveLastToFrontOf(node);
		if(node->GetParentPageId() != INVALID_PAGE_ID) {
			auto parent_page = buffer_pool_manager_->FetchPage(node->GetParentPageId());
			auto parent = reinterpret_cast<InternalPage *>(parent_page->GetData());
			parent->SetKeyAt(parent->ValueIndex(node->GetPageId()), node->KeyAt(0));
			buffer_pool_manager_->UnpinPage(parent->GetPageId(), true);
		}
	}
}
void BPlusTree::Redistribute(InternalPage *neighbor_node, InternalPage *node, int index) {
	auto parent_page = buffer_pool_manager_->FetchPage(node->GetParentPageId());
	auto parent = reinterpret_cast<InternalPage *>(parent_page->GetData());
	if(index == 0) {
		auto middle_key = parent->KeyAt(parent->ValueIndex(neighbor_node->GetPageId()));
		neighbor_node->MoveFirstToEndOf(node, middle_key, buffer_pool_manager_);
		if(neighbor_node->GetParentPageId() != INVALID_PAGE_ID) {
			auto grand_parent_page = buffer_pool_manager_->FetchPage(neighbor_node->GetParentPageId());
			auto grand_parent = reinterpret_cast<InternalPage *>(grand_parent_page->GetData());
			grand_parent->SetKeyAt(grand_parent->ValueIndex(neighbor_node->GetPageId()), neighbor_node->KeyAt(0));
			buffer_pool_manager_->UnpinPage(grand_parent->GetPageId(), true);
		}
	} else {
		auto middle_key = parent->KeyAt(parent->ValueIndex(node->GetPageId()));
		neighbor_node->MoveLastToFrontOf(node, middle_key, buffer_pool_manager_);
		if(node->GetParentPageId() != INVALID_PAGE_ID) {
			auto grand_parent_page = buffer_pool_manager_->FetchPage(node->GetParentPageId());
			auto grand_parent = reinterpret_cast<InternalPage *>(grand_parent_page->GetData());
			grand_parent->SetKeyAt(grand_parent->ValueIndex(node->GetPageId()), node->KeyAt(0));
			buffer_pool_manager_->UnpinPage(grand_parent->GetPageId(), true);
		}
	}
	buffer_pool_manager_->UnpinPage(parent->GetPageId(), true);
}
/*
 * Update root page if necessary
 * NOTE: size of root page can be less than min size and this method is only
 * called within coalesceOrRedistribute() method
 * case 1: when you delete the last element in root page, but root page still
 * has one last child
 * case 2: when you delete the last element in whole b+ tree
 * @return : true means root page should be deleted, false means no deletion
 * happened
 */
bool BPlusTree::AdjustRoot(BPlusTreePage *old_root_node) {
	if(!old_root_node->IsLeafPage() && old_root_node->GetSize() == 1) {
		auto root_node = reinterpret_cast<InternalPage *>(old_root_node);
		page_id_t root_page_id = root_node->RemoveAndReturnOnlyChild();
		auto child_page = buffer_pool_manager_->FetchPage(root_page_id);
		auto child_node = reinterpret_cast<BPlusTreePage *>(child_page->GetData());

		child_node->SetParentPageId(INVALID_PAGE_ID);
		root_page_id_ = root_page_id;
		root_page_id_ = child_node->GetPageId();
		buffer_pool_manager_->UnpinPage(root_page_id_, true);
		UpdateRootPageId(0);
		return true;
	} else {
		root_page_id_ = INVALID_PAGE_ID;
		UpdateRootPageId(0);
	}
	return true;
}

/****************************************************************************
 * INDEX ITERATOR
 *****************************************************************************/
/*
 * Input parameter is void, find the left most leaf page first, then construct
 * index iterator
 * @return : index iterator
 */
IndexIterator BPlusTree::Begin() {
	auto leaf_page = FindLeafPage(nullptr, root_page_id_, true);
	auto leaf = reinterpret_cast<LeafPage *>(leaf_page->GetData());
	buffer_pool_manager_->UnpinPage(leaf->GetPageId(), false);
	return IndexIterator(leaf->GetPageId(), buffer_pool_manager_);
}

/*
 * Input parameter is low-key, find the leaf page that contains the input key
 * first, then construct index iterator
 * @return : index iterator
 */
IndexIterator BPlusTree::Begin(const GenericKey *key) {
	auto leaf_page = FindLeafPage(key, root_page_id_, false);
	auto leaf_node = reinterpret_cast<LeafPage *>(leaf_page->GetData());
	buffer_pool_manager_->UnpinPage(leaf_node->GetPageId(), false);
	return IndexIterator(leaf_node->GetPageId(), buffer_pool_manager_, leaf_node->KeyIndex(key, processor_));
}

/*
 * Input parameter is void, construct an index iterator representing the end
 * of the key/value pair in the leaf node
 * @return : index iterator
 */
IndexIterator BPlusTree::End() {
//	auto leaf_page = FindRightMostLeafPage(root_page_id_);
//	auto leaf_node = reinterpret_cast<LeafPage *>(leaf_page->GetData());
//	return IndexIterator(leaf_page->GetPageId(), buffer_pool_manager_, leaf_node->GetSize());
	return IndexIterator(INVALID_PAGE_ID,buffer_pool_manager_,0);
}

/*****************************************************************************
 * UTILITIES AND DEBUG
 *****************************************************************************/
/*
 * Find leaf page containing particular key, if leftMost flag == true, find
 * the left most leaf page
 * Note: the leaf page is pinned, you need to unpin it after use.
 */
Page *BPlusTree::FindLeafPage(const GenericKey *key, page_id_t page_id, bool leftMost) {
	auto page = buffer_pool_manager_->FetchPage(page_id);
	auto node = reinterpret_cast<BPlusTreePage *>(page->GetData());
	page->RLatch();
	if(node->IsLeafPage()) {
		return page;
	}
	auto internal = reinterpret_cast<InternalPage *>(node);
	page_id_t child_id;
	if(leftMost) {
		child_id = internal->ValueAt(0);
	} else {
		child_id = internal->Lookup(key, processor_);
	}
	page->RUnlatch();
	buffer_pool_manager_->UnpinPage(page_id, false);
	return FindLeafPage(key, child_id, leftMost);
}

/*
 * Find leaf page containing particular key, if leftMost flag == true, find
 * the left most leaf page
 * Note: the leaf page is pinned, you need to unpin it after use.
 */
Page *BPlusTree::FindRightMostLeafPage(page_id_t page_id) {
	auto page = buffer_pool_manager_->FetchPage(page_id);
	auto node = reinterpret_cast<BPlusTreePage *>(page->GetData());
	page->RLatch();
	if(node->IsLeafPage()) {
		return page;
	}
	auto internal = reinterpret_cast<InternalPage *>(node);
	page_id_t child_id = internal->ValueAt(internal->GetSize() - 1);
	page->RUnlatch();
	buffer_pool_manager_->UnpinPage(page_id, false);
	return FindRightMostLeafPage(child_id);
}

/*
 * Update/Insert root page id in header page(where page_id = 0, header_page is
 * defined under include/page/header_page.h)
 * Call this method everytime root page id is changed.
 * @parameter: insert_record      default value is false. When set to true,
 * insert a record <index_name, current_page_id> into header page instead of
 * updating it.
 */
void BPlusTree::UpdateRootPageId(int insert_record) {
	auto header_page = buffer_pool_manager_->FetchPage(INDEX_ROOTS_PAGE_ID);
	auto header = reinterpret_cast<IndexRootsPage *>(header_page->GetData());
	if(insert_record) {
		header->Insert(index_id_, root_page_id_);
	} else {
		header->Update(index_id_, root_page_id_);
	}
	buffer_pool_manager_->UnpinPage(INDEX_ROOTS_PAGE_ID, true);
}

/**
 * This method is used for debug only, You don't need to modify
 */
void BPlusTree::ToGraph(BPlusTreePage *page, BufferPoolManager *bpm, std::ofstream &out) const {
  std::string leaf_prefix("LEAF_");
  std::string internal_prefix("INT_");
  if (page->IsLeafPage()) {
    auto *leaf = reinterpret_cast<LeafPage *>(page);
    // Print node name
    out << leaf_prefix << leaf->GetPageId();
    // Print node properties
    out << "[shape=plain color=green ";
    // Print data of the node
    out << "label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n";
    // Print data
    out << "<TR><TD COLSPAN=\"" << leaf->GetSize() << "\">P=" << leaf->GetPageId()
        << ",Parent=" << leaf->GetParentPageId() << "</TD></TR>\n";
    out << "<TR><TD COLSPAN=\"" << leaf->GetSize() << "\">"
        << "max_size=" << leaf->GetMaxSize() << ",min_size=" << leaf->GetMinSize() << ",size=" << leaf->GetSize()
        << "</TD></TR>\n";
    out << "<TR>";
    for (int i = 0; i < leaf->GetSize(); i++) {
      out << "<TD>" << leaf->KeyAt(i) << "</TD>\n";
    }
    out << "</TR>";
    // Print table end
    out << "</TABLE>>];\n";
    // Print Leaf node link if there is a next page
    if (leaf->GetNextPageId() != INVALID_PAGE_ID) {
      out << leaf_prefix << leaf->GetPageId() << " -> " << leaf_prefix << leaf->GetNextPageId() << ";\n";
      out << "{rank=same " << leaf_prefix << leaf->GetPageId() << " " << leaf_prefix << leaf->GetNextPageId() << "};\n";
    }

    // Print parent links if there is a parent
    if (leaf->GetParentPageId() != INVALID_PAGE_ID) {
      out << internal_prefix << leaf->GetParentPageId() << ":p" << leaf->GetPageId() << " -> " << leaf_prefix
          << leaf->GetPageId() << ";\n";
    }
  } else {
    auto *inner = reinterpret_cast<InternalPage *>(page);
    // Print node name
    out << internal_prefix << inner->GetPageId();
    // Print node properties
    out << "[shape=plain color=pink ";  // why not?
    // Print data of the node
    out << "label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n";
    // Print data
    out << "<TR><TD COLSPAN=\"" << inner->GetSize() << "\">P=" << inner->GetPageId()
        << ",Parent=" << inner->GetParentPageId() << "</TD></TR>\n";
    out << "<TR><TD COLSPAN=\"" << inner->GetSize() << "\">"
        << "max_size=" << inner->GetMaxSize() << ",min_size=" << inner->GetMinSize() << ",size=" << inner->GetSize()
        << "</TD></TR>\n";
    out << "<TR>";
    for (int i = 0; i < inner->GetSize(); i++) {
      out << "<TD PORT=\"p" << inner->ValueAt(i) << "\">";
      if (i > 0) {
        out << inner->KeyAt(i);
      } else {
        out << " ";
      }
      out << "</TD>\n";
    }
    out << "</TR>";
    // Print table end
    out << "</TABLE>>];\n";
    // Print Parent link
    if (inner->GetParentPageId() != INVALID_PAGE_ID) {
      out << internal_prefix << inner->GetParentPageId() << ":p" << inner->GetPageId() << " -> " << internal_prefix
          << inner->GetPageId() << ";\n";
    }
    // Print leaves
    for (int i = 0; i < inner->GetSize(); i++) {
      auto child_page = reinterpret_cast<BPlusTreePage *>(bpm->FetchPage(inner->ValueAt(i))->GetData());
      ToGraph(child_page, bpm, out);
      if (i > 0) {
        auto sibling_page = reinterpret_cast<BPlusTreePage *>(bpm->FetchPage(inner->ValueAt(i - 1))->GetData());
        if (!sibling_page->IsLeafPage() && !child_page->IsLeafPage()) {
          out << "{rank=same " << internal_prefix << sibling_page->GetPageId() << " " << internal_prefix
              << child_page->GetPageId() << "};\n";
        }
        bpm->UnpinPage(sibling_page->GetPageId(), false);
      }
    }
  }
  bpm->UnpinPage(page->GetPageId(), false);
}

/**
 * This function is for debug only, you don't need to modify
 */
void BPlusTree::ToString(BPlusTreePage *page, BufferPoolManager *bpm) const {
	if (page->IsLeafPage()) {
		auto *leaf = reinterpret_cast<LeafPage *>(page);
		std::cout << "Leaf Page: " << leaf->GetPageId() << " parent: " << leaf->GetParentPageId()
				  << " next: " << leaf->GetNextPageId() << std::endl;
		for (int i = 0; i < leaf->GetSize(); i++) {
			std::cout << leaf->ValueAt(i).Get() << ",";
		}
		std::cout << std::endl;
		std::cout << std::endl;
	} else {
		auto *internal = reinterpret_cast<InternalPage *>(page);
		std::cout << "Internal Page: " << internal->GetPageId() << " parent: " << internal->GetParentPageId() << std::endl;
		for (int i = 0; i < internal->GetSize(); i++) {
			std::cout << internal->KeyAt(i) << ": " << internal->ValueAt(i) << ",";
		}
		std::cout << std::endl;
		std::cout << std::endl;
		for (int i = 0; i < internal->GetSize(); i++) {
			ToString(reinterpret_cast<BPlusTreePage *>(bpm->FetchPage(internal->ValueAt(i))->GetData()), bpm);
			bpm->UnpinPage(internal->ValueAt(i), false);
		}
	}
}


bool BPlusTree::Check() {
  bool all_unpinned = buffer_pool_manager_->CheckAllUnpinned();
  if (!all_unpinned) {
    LOG(ERROR) << "problem in page unpin" << endl;
  }
  return all_unpinned;
}