#include "rbtree.h"

#include <stddef.h>

#define RB_COLOR_RED 0
#define RB_COLOR_BLACK 1

#define rbtreeIsRed(node) ((node) != NULL && (node)->pcolor == RB_COLOR_RED)

struct RBTree {
	RBTreeNode *root;
	size_t size;

	void *(*alloc)(size_t);
	void (*free_key)(void *key);
	void (*free_value)(void *value);
	int (*compare)(void *key1, void *key2);
};

struct RBTreeNode {
	void *key;
	void *value;
	int pcolor;
	struct RBTreeNode *left;
	struct RBTreeNode *right;
};

static void flipColors(RBTreeNode *node)
{
	node->pcolor = RB_COLOR_RED;
	node->left->pcolor = RB_COLOR_BLACK;
	node->right->pcolor = RB_COLOR_BLACK;
}

static RBTreeNode *rotateLeft(RBTreeNode *node)
{
	RBTreeNode *right = node->right;
	node->right = right->left;
	right->left = node;
	right->pcolor = node->pcolor;
	node->pcolor = RB_COLOR_RED;
	return right;
}

static RBTreeNode *rotateRight(RBTreeNode *node)
{
	RBTreeNode *left = node->left;
	node->left = left->right;
	left->right = node;
	left->pcolor = node->pcolor;
	node->pcolor = RB_COLOR_RED;
	return left;
}

static RBTreeNode *_rbtreeInsert(RBTree *tree, RBTreeNode *node, void *key,
				 void *value)
{
	if (node == NULL) {
		node = tree->alloc(sizeof(RBTreeNode));
		node->key = key;
		node->value = value;
		node->pcolor = RB_COLOR_RED;
		node->left = NULL;
		node->right = NULL;
		return node;
	}

	int cmp = tree->compare(node->key, key);
	if (cmp == 0) {
		if (tree->free_value != NULL) {
			tree->free_value(node->value);
		}

		node->value = value;
		return node;
	}

	if (cmp > 0) {
		node->left = _rbtreeInsert(tree, node, key, value);
	} else {
		node->right = _rbtreeInsert(tree, node, key, value);
	}

	if (rbtreeIsRed(node->left) && rbtreeIsRed(node->left->right)) {
		node = rotateLeft(node->left);
	}

	if (rbtreeIsRed(node->left) && rbtreeIsRed(node->left->left)) {
		node = rotateRight(node);
	}

	if (rbtreeIsRed(node->left) && rbtreeIsRed(node->right)) {
		flipColors(node);
	}

	return node;
}

size_t rbtreeSize(RBTree *tree) { return tree->size; }

RBTree *rbtreeInsert(RBTree *tree, void *key, void *value)
{
	tree->root = _rbtreeInsert(tree, tree->root, key, value);
	tree->root->pcolor = RB_COLOR_BLACK;
	return tree;
}
