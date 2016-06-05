#include "rbtree.h"

#include <stddef.h>
#include <string.h>

#define RB_COLOR_RED 0
#define RB_COLOR_BLACK 1

typedef struct RBTreeNode RBTreeNode;

struct RBTreeNode {
	void *key;
	void *value;
	int color;
	RBTreeNode *parent;
	RBTreeNode *left;
	RBTreeNode *right;
};

struct RBTree {
	RBTreeNode *root;
	size_t size;

	void *(*alloc)(size_t);
	void (*dealloc)(void *);
	void (*free_key)(void *);
	void (*free_value)(void *);
	int (*compare)(void *, void *);
};

struct RBTreeIter {
	RBTreeNode *next;
	void (*dealloc)(void *);
};

#define rbtreeIsRed(node) ((node) != NULL && (node)->color == RB_COLOR_RED)

RBTree *rbtreeCreate(void *(*alloc)(size_t), void (*dealloc)(void *))
{
	RBTree *tree = alloc(sizeof(RBTree));
	memset(tree, 0, sizeof(RBTree));
	tree->alloc = alloc;
	tree->dealloc = dealloc;
	return tree;
}

void (*rbtreeGetFreeKeyMethod(RBTree *tree))(void *key)
{
	return tree->free_key;
}

void rbtreeSetFreeKeyMethod(RBTree *tree, void (*free_key)(void *))
{
	tree->free_key = free_key;
}

void (*rbtreeGetFreeValueMethod(RBTree *tree))(void *value)
{
	return tree->free_value;
}

void rbtreeSetFreeValueMethod(RBTree *tree, void (*free_value)(void *))
{
	tree->free_value = free_value;
}

int (*rbtreeGetCompareMethod(RBTree *tree))(void *key1, void *key2)
{
	return tree->compare;
}

void rbtreeSetCompareMethod(RBTree *tree, int (*compare)(void *, void *))
{
	tree->compare = compare;
}

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

static RBTreeNode *successor(RBTreeNode *node)
{
	if (node == NULL) {
		return NULL;
	}

	if (node->right != NULL) {
		node = node->right;
		while (node->left != NULL) {
			node = node->left;
		}
	} else {
		RBTreeNode *parent = node->parent;
		while (parent != NULL && node == parent->right) {
			node = parent;
			parent = parent->parent;
		}

		node = parent;
	}

	return node;
}

int rbtreeContains(RBTree *tree, void *key)
{
	return rbtreeGet(tree, key) != NULL;
}

void *rbtreeGet(RBTree *tree, void *key)
{
	RBTreeNode *node = tree->root;
	int cmp;
	while (node != NULL) {
		cmp = tree->compare(key, node->key);
		if (cmp == 0) {
			break;
		} else if (cmp < 0) {
			node = node->left;
		} else {
			node = node->right;
		}
	}

	if (node == NULL) {
		return NULL;
	}

	return node->value;
}

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

void rbtreeSet(RBTree *tree, void *key, void *value)
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
			return;
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

static void rbtreeFreeNode(RBTree *tree, RBTreeNode *node)
{
	if (tree->free_key != NULL) {
		tree->free_key(node->key);
	}

	if (tree->free_value != NULL) {
		tree->free_value(node->value);
	}

	tree->dealloc(node);
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

void *rbtreeRemove(RBTree *tree, void *key)
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

	void *value = node->value;

	if (tree->free_key != NULL) {
		tree->free_key(node->key);
	}

	tree->dealloc(node);

	if (color == RB_COLOR_BLACK) {
		delFixUp(tree, fixUpNode);
	}

	--tree->size;
	return value;
}

void rbtreeDel(RBTree *tree, void *key)
{
	void *value = rbtreeRemove(tree, key);
	if (tree->free_value != NULL) {
		tree->free_value(value);
	}
}

// Morris Postorder Traversal
void rbtreeClear(RBTree *tree)
{
	RBTreeNode node;
	node.left = tree->root;
	node.right = NULL;
	node.parent = NULL;

	RBTreeNode *current = &node;
	RBTreeNode *tmp = NULL;
	while (current != NULL) {
		if ((tmp = current->left) != NULL) {
			while (tmp->right != NULL && tmp->right != current) {
				tmp = tmp->right;
			}

			if (tmp->right == NULL) {
				tmp->right = current;
				current = current->left;
			} else {
				RBTreeNode *tmpParent = NULL;
				while (tmp != current) {
					tmpParent = tmp->parent;
					rbtreeFreeNode(tree, tmp);
					tmp = tmpParent;
				}

				current = current->right;
			}
		} else {
			current = current->right;
		}
	}

	tree->root = NULL;
	tree->size = 0;
}

void rbtreeDestroy(RBTree *tree)
{
	rbtreeClear(tree);
	tree->dealloc(tree);
}

RBTreeIter *rbtreeIterator(RBTree *tree)
{
	RBTreeIter *iter = tree->alloc(sizeof(RBTreeIter));
	iter->dealloc = tree->dealloc;
	iter->next = rbtreeMinNode(tree, tree->root);
	return iter;
}

int rbtreeIterHasNext(RBTreeIter *iter) { return iter->next != NULL; }

void rbtreeIterNext(RBTreeIter *iter, void **key_ptr, void **value_ptr)
{
	*key_ptr = iter->next->key;
	*value_ptr = iter->next->value;
	iter->next = successor(iter->next);
}

void rbtreeIterDestroy(RBTreeIter *iter) { iter->dealloc(iter); }
