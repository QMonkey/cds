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
	RBTreeNode *tmpNode = NULL;

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
				tmpNode = node;
				node = parent;
				parent = tmpNode;
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
				tmpNode = node;
				node = parent;
				parent = tmpNode;
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

static void transplant(RBTree *tree, RBTreeNode *dest, RBTreeNode *src)
{
	if (dest->parent == NULL) {
		tree->root = src;
	} else if (dest == dest->parent->left) {
		dest->parent->left = src;
	} else {
		dest->parent->right = src;
	}

	if (src != NULL) {
		src->color = dest->color;
		src->parent = dest->parent;
	}
}

static RBTreeNode *rbtreeMinNode(RBTree *tree, RBTreeNode *root)
{
	while (root->left != NULL) {
		root = root->left;
	}

	return root;
}

static void delFixUp(RBTree *tree, RBTreeNode *node)
{
	RBTreeNode *parent = NULL;
	while (node != tree->root) {
		if (rbtreeIsRed(node)) {
			node->color = RB_COLOR_BLACK;
			break;
		}

		parent = node->parent;
		if (parent == NULL) {
			break;
		}

		RBTreeNode *brother = NULL;
		if (node == parent->left) {
			brother = parent->right;
			if (rbtreeIsRed(brother)) {
				rotateLeft(tree, parent);
				parent->color = RB_COLOR_RED;
				brother->color = RB_COLOR_BLACK;
				brother = parent->right;
			}

			if (!rbtreeIsRed(brother->left) &&
			    !rbtreeIsRed(brother->right)) {
				brother->color = RB_COLOR_RED;
				node = parent;
			} else {
				if (rbtreeIsRed(node->left)) {
					brother->left->color = RB_COLOR_BLACK;
					brother->color = RB_COLOR_RED;
					rotateRight(tree, brother);
					brother = parent->right;
				}

				brother->right->color = RB_COLOR_BLACK;
				brother->color = parent->color;
				parent->color = RB_COLOR_BLACK;
				rotateLeft(tree, parent);
				node = tree->root;
			}
		} else {
			brother = parent->left;
			if (rbtreeIsRed(brother)) {
				rotateRight(tree, parent);
				parent->color = RB_COLOR_RED;
				brother->color = RB_COLOR_BLACK;
				brother = parent->left;
			}

			if (!rbtreeIsRed(brother->left) &&
			    !rbtreeIsRed(brother->right)) {
				brother->left->color = RB_COLOR_RED;
				node = parent;
			} else {
				if (rbtreeIsRed(brother->right)) {
					brother->right->color = RB_COLOR_BLACK;
					brother->color = RB_COLOR_RED;
					rotateLeft(tree, brother);
					brother = parent->left;
				}

				brother->color = parent->color;
				parent->color = RB_COLOR_BLACK;
				brother->left->color = RB_COLOR_BLACK;
				rotateRight(tree, parent);
				node = tree->root;
			}
		}
	}

	tree->root->color = RB_COLOR_BLACK;
}

RBTree *rbtreeDel(RBTree *tree, void *key)
{
	RBTreeNode *node = tree->root;
	int cmp;
	while (node != NULL && (cmp = tree->compare(key, node->key)) != 0) {
		if (cmp < 0) {
			node = node->left;
		} else {
			node = node->right;
		}
	}

	if (node == NULL) {
		return tree;
	}

	int color = node->color;
	RBTreeNode *fixUpNode = NULL;

	if (node->left == NULL) {
		fixUpNode = node->right;
	} else if (node->right == NULL) {
		fixUpNode = node->left;
	} else {
		RBTreeNode *tmpNode = rbtreeMinNode(tree, node->right);
		if (tmpNode->parent == node) {
			fixUpNode = tmpNode;
		} else {
			fixUpNode = tmpNode->right;
			color = tmpNode->color;

			void *tmp = NULL;
			tmp = node->key;
			node->key = tmpNode->key;
			tmpNode->key = tmp;

			tmp = node->value;
			node->value = tmpNode->value;
			tmpNode->value = tmp;
			node = tmpNode;
		}
	}

	transplant(tree, node, fixUpNode);

	if (tree->free_key != NULL) {
		tree->free_key(node->key);
	}

	if (tree->free_value != NULL) {
		tree->free_value(node->value);
	}

	tree->dealloc(node);

	if (color == RB_COLOR_BLACK) {
		delFixUp(tree, fixUpNode);
	}

	--tree->size;
	return tree;
}
