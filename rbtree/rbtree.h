#ifndef RBTREE_H
#define RBTREE_H

typedef struct RBTree RBTree;
typedef struct RBTreeNode RBTreeNode;

RBTree *rbtreeInsert(RBTree *tree, void *key, void *value);

#endif
