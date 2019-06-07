#include <gtest/gtest.h>

#include "suffix-tree/suffix-tree.h"

/*
BEFORE root->FindPath("aaazzz")
root
    \
     \ aaabbbaaa
      \
     leaf0

AFTER root->FindPath("aaazzz")
root
    \
     \ aaa
      \ 
       new_inner
       /       \
  zzz /         \ bbbaaa
     /           \
leaf1            leaf0
*/
TEST(SuffixTreeFindPath, QueryMismatchInEdge) {
	std::string genome = "aaabbbaaa";
	std::string query = "aaazzz";
	suffix_tree::SuffixTreeNode * root = new suffix_tree::SuffixTreeNode("root", 4, nullptr);
	root->parent_ = root;
	suffix_tree::SuffixTreeNode * leaf0 = new suffix_tree::SuffixTreeNode(genome.c_str(), genome.size(), root);
	auto leaf1 = root->FindPath(query.c_str(), query.size());

	// verify children parent relationships
	EXPECT_FALSE(leaf1 == nullptr);

	EXPECT_EQ(1, root->children_.size());
	auto search = root->children_.find('a');
	EXPECT_FALSE(search == root->children_.end());
	auto new_inner = search->second;
	EXPECT_EQ(new_inner, leaf0->parent_);
	EXPECT_EQ(new_inner, leaf1->parent_);
	EXPECT_EQ(new_inner->parent_, root);

	EXPECT_EQ(2, new_inner->children_.size());
	// TODO verify *which* children
	// TODO str_depths

	// verify edges
	EXPECT_EQ(3, new_inner->edge_len_);
	EXPECT_EQ(0, strncmp("aaa", new_inner->incoming_edge_label_, new_inner->edge_len_));

	EXPECT_EQ(6, leaf0->edge_len_);
	EXPECT_EQ(0, strncmp("bbbaaa", leaf0->incoming_edge_label_, leaf0->edge_len_));

	EXPECT_EQ(3, leaf1->edge_len_);
	EXPECT_EQ(0, strncmp("zzz", leaf1->incoming_edge_label_, leaf1->edge_len_));

	// delete root (which deletes its children)
	delete root;
}

/*
root
 \
  \ aaa
   \
   inner_node
       \
        \ bbb
         \
        leaf0

root
 \
  \ aaa
   \
   inner_node
      /     \
'\0' /       \ bbb
    /         \
 leaf1      leaf0
*/
TEST(SuffixTreeFindPath, QueryEndsAtNode) {
	auto root = new suffix_tree::SuffixTreeNode("root", 4, nullptr);
	root->parent_ = root;
	auto inner_node = new suffix_tree::SuffixTreeNode("aaa", 3, root);
	auto leaf0 = new suffix_tree::SuffixTreeNode("bbb", 3, inner_node);
	auto leaf1 = root->FindPath("aaa", 3);
	ASSERT_FALSE(leaf1 == nullptr);

	// verify child-parent relation
	EXPECT_EQ(inner_node, leaf1->parent_);
	auto search = inner_node->children_.find('\0');
	EXPECT_FALSE(search == inner_node->children_.end());
	EXPECT_EQ(leaf1, search->second);

	EXPECT_EQ(0, leaf1->edge_len_);

	delete root;
}


/*
BEFORE root->FindPath("aaa")
root
    \
     \ aaabbbaaa
      \
     leaf0

AFTER root->FindPath("aaa")
root
    \
     \ aaa
      \ 
       new_inner
       /       \
 '\0' /         \ bbbaaa
     /           \
leaf1            leaf0
*/
TEST(SuffixTreeFindPath, QueryEndsInEdge) {
	std::string genome = "aaabbbaaa";
	std::string query = "aaa";
	suffix_tree::SuffixTreeNode * root = new suffix_tree::SuffixTreeNode("root", 4, nullptr);
	root->parent_ = root;
	suffix_tree::SuffixTreeNode * leaf0 = new suffix_tree::SuffixTreeNode(genome.c_str(), genome.size(), root);
	auto leaf1 = root->FindPath(query.c_str(), query.size());

	// verify children parent relationships
	EXPECT_FALSE(leaf1 == nullptr);

	EXPECT_EQ(1, root->children_.size());
	auto search = root->children_.find('a');
	EXPECT_FALSE(search == root->children_.end());
	auto new_inner = search->second;
	EXPECT_EQ(new_inner, leaf0->parent_);
	EXPECT_EQ(new_inner, leaf1->parent_);
	EXPECT_EQ(new_inner->parent_, root);

	EXPECT_EQ(2, new_inner->children_.size());
	// TODO verify *which* children
	// TODO str_depths

	// verify edges
	EXPECT_EQ(3, new_inner->edge_len_);
	EXPECT_EQ(0, strncmp("aaa", new_inner->incoming_edge_label_, new_inner->edge_len_));

	EXPECT_EQ(6, leaf0->edge_len_);
	EXPECT_EQ(0, strncmp("bbbaaa", leaf0->incoming_edge_label_, leaf0->edge_len_));

	EXPECT_EQ(0, leaf1->edge_len_);

	// delete root (which deletes its children)
	delete root;
}

TEST(SuffixTreeNodeHops, BetaEndsAtNode) {
	auto root = new suffix_tree::SuffixTreeNode("root", 4, nullptr);
	root->parent_ = root;
	auto inner_node = new suffix_tree::SuffixTreeNode("aaa", 3, root);
	auto leaf0 = new suffix_tree::SuffixTreeNode("bbb", 3, inner_node);
	auto hop_result = root->NodeHops("aaabbb", 6);
	EXPECT_EQ(leaf0, hop_result);

	delete root;
}

/*
root
 \
  \ aaa
   \
   inner_node
       \
        \ bbb
         \
        leaf


root
 \
  \ aaa
   \
   inner_node
       \
        \ bb
         \
    new_inner_node
          \
           \ b
            \
           leaf

*/
TEST(SuffixTreeNodeHops, BetaEndsInEdge) {
	auto root = new suffix_tree::SuffixTreeNode("root", 4, nullptr);
	root->parent_ = root;
	auto inner_node = new suffix_tree::SuffixTreeNode("aaa", 3, root);
	auto leaf = new suffix_tree::SuffixTreeNode("bbb", 3, inner_node);
	auto new_inner_node = root->NodeHops("aaabb", 5);

	EXPECT_EQ(leaf->parent_, new_inner_node);
	EXPECT_EQ(new_inner_node->parent_, inner_node);

	EXPECT_EQ(1, leaf->edge_len_);
	EXPECT_EQ(2, new_inner_node->edge_len_);
	
	delete root;
}

/*
([7] root SL->[7]) (root)
  ([6]  )
  ([8] a SL->[7])
    ([5]  )
    ([9] na SL->[10])
      ([3]  )
      ([1]  na)
  ([0]  banana)
  ([10] na SL->[8])
    ([4]  )
    ([2]  na)
*/
// depends on BuildTreeMccreight() and MatchStr()
TEST(SuffixTree, BananaCase) {
	std::string genome = "banana";
	const char * bps = genome.c_str();

	suffix_tree::SuffixTree* st = new suffix_tree::SuffixTree(genome.c_str(), genome.size());
	// st->PrintTree();
	auto root = st->root_;
	EXPECT_FALSE(root == nullptr);
	
	// ensure each suffix exists in the suffix tree
	for (size_t i = 0; i < genome.size() + 1; i++) {
		int match_len = 0;
		auto matched_node = root->MatchStr(bps + i, genome.size() - i, match_len);
		EXPECT_FALSE(matched_node == nullptr);
		EXPECT_EQ(match_len, genome.size() - i);
	}

	EXPECT_EQ(4, root->children_.size());
	delete st;
}

/*
([12] root SL->[12]) (root)
  ([11]  )
  ([13] i SL->[12])
    ([10]  )
    ([7]  ppi)
    ([14] ssi SL->[18])
      ([4]  ppi)
      ([1]  ssippi)
  ([0]  mississippi)
  ([15] p SL->[12])
    ([9]  i)
    ([8]  pi)
  ([16] s SL->[12])
    ([17] i SL->[13])
      ([6]  ppi)
      ([3]  ssippi)
    ([18] si SL->[17])
      ([5]  ppi)
      ([2]  ssippi)
*/
// depends on BuildTreeMccreight() and MatchStr()
TEST(SuffixTree, MississippiCase) {
	std::string genome = "mississippi";
	const char * bps = genome.c_str();

	suffix_tree::SuffixTree* st = new suffix_tree::SuffixTree(genome.c_str(), genome.size());
	// st->PrintTree();
	auto root = st->root_;
	EXPECT_FALSE(root == nullptr);
	
	// ensure each suffix exists in the suffix tree
	for (size_t i = 0; i < genome.size() + 1; i++) {
		int match_len = 0;
		auto matched_node = root->MatchStr(bps + i, genome.size() - i, match_len);
		EXPECT_FALSE(matched_node == nullptr);
		EXPECT_EQ(match_len, genome.size() - i);
	}

	EXPECT_EQ(5, root->children_.size());
	delete st;
}
