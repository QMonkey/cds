#include "rbtree.h"

#include <stddef.h>

#define RB_COLOR_RED 0
#define RB_COLOR_BLACK 1

#define rbtreeIsRed(node) ((node) != NULL && (node)->color == RB_COLOR_RED)

struct RBTree {
	RBTreeNode *root;
	size_t size;

	void *(*alloc)(size_t);
	void (*dealloc)(void *);
	void (*free_key)(void *key);
	void (*free_value)(void *value);
	int (*compare)(void *key1, void *key2);
};

struct RBTreeNode {
	void *key;
	void *value;
	int color;
	struct RBTreeNode *parent;
	struct RBTreeNode *left;
	struct RBTreeNode *right;
};

static void rotateLeft(RBTree *tree, RBTreeNode *node)
{
	RBTreeNode *parent = node->parent;
	RBTreeNode *right = node->right;
	if (tree->root == node) {
		tree->root = right;
	}
	node->parent = right;
	right->parent = parent;
	node->right = right->left;
	right->left = node;
}

static void rotateRight(RBTree *tree, RBTreeNode *node)
{
	RBTreeNode *parent = node->parent;
	RBTreeNode *left = node->left;
	if (tree->root == node) {
		tree->root = left;
	}
	node->parent = left;
	left->parent = parent;
	node->left = left->right;
	left->right = node;
}

size_t rbtreeSize(RBTree *tree) { return tree->size; }

static void insertFixUp(RBTree *tree, RBTreeNode *node)
{
	RBTreeNode *parent = NULL;
	RBTreeNode *grandparent = NULL;
	RBTreeNode *uncle = NULL;
	RBTreeNode *tmp = NULL;

	while (rbtreeIsRed(node)) {
		parent = node->parent;
		if (parent == NULL || !rbtreeIsRed(parent)) {
			break;
		}

		grandparent = parent->parent;
		if (grandparent == NULL) {
			break;
		}

		if (grandparent->left == parent) {
			uncle = grandparent->right;
			if (node == parent->right) {
				rotateLeft(tree, parent);
				tmp = node;
				node = parent;
				parent = tmp;
			}

			if (rbtreeIsRed(uncle)) {
				parent->color = RB_COLOR_BLACK;
				uncle->color = RB_COLOR_BLACK;
				grandparent->color = RB_COLOR_RED;
				node = grandparent;
			} else {
				rotateRight(tree, grandparent);
				parent->color = RB_COLOR_BLACK;
				grandparent->color = RB_COLOR_RED;
				node = parent;
			}
		} else {
			uncle = grandparent->left;
			if (node == parent->left) {
				rotateRight(tree, parent);
				tmp = node;
				node = parent;
				parent = tmp;
			}

			if (rbtreeIsRed(uncle)) {
				parent->color = RB_COLOR_BLACK;
				uncle->color = RB_COLOR_BLACK;
				grandparent->color = RB_COLOR_RED;
				node = grandparent;
			} else {
				rotateLeft(tree, grandparent);
				parent->color = RB_COLOR_BLACK;
				grandparent->color = RB_COLOR_RED;
				node = parent;
			}
		}
	}

	tree->root->color = RB_COLOR_BLACK;
}

RBTree *rbtreeInsert(RBTree *tree, void *key, void *value)
{
	RBTreeNode *later = NULL;
	RBTreeNode *current = tree->root;
	int cmp;
	while (current != NULL) {
		cmp = tree->compare(key, current->key);
		if (cmp == 0) {
			if (tree->free_value != NULL) {
				tree->free_value(current->value);
			}
			current->value = value;
			return tree;
		}

		if (cmp < 0) {
			later = current;
			current = current->left;
		} else {
			later = current;
			current = current->right;
		}
	}

	current = tree->alloc(sizeof(RBTreeNode));
	current->key = key;
	current->value = value;
	current->color = RB_COLOR_RED;
	current->parent = later;
	current->left = NULL;
	current->right = NULL;

	if (tree->root == NULL) {
		tree->root = current;
	} else {
		if (cmp < 0) {
			later->left = current;
		} else {
			later->right = current;
		}
	}

	insertFixUp(tree, current);

	++tree->size;
	return tree;
}

static void delFixUp(RBTree *tree, RBTreeNode *node) {}

RBTree *rbtreeDel(RBTree *tree, void *key)
{
	// TODO
	--tree->size;
	return tree;
}
