#ifndef HTABLE_H
#define HTABLE_H

#include <stddef.h>

typedef struct HashTable HashTable;
typedef struct HashTableIter HashTableIter;

HashTable *hashTableCreate(void *(*alloc)(size_t), void (*dealloc)(void *));
size_t (*getHashMethod(HashTable *htable))(void *);
void setHashMethod(HashTable *htable, size_t (*hash)(void *));
int (*getCompareMethod(HashTable *htable))(void *, void *);
void setCompareMethod(HashTable *htable, int (*compare)(void *, void *));
void (*getFreeKeyMethod(HashTable *htable))(void *);
void setFreeKeyMethod(HashTable *htable, void (*free_key)(void *));
void (*getFreeValueMethod(HashTable *htable))(void *);
void setFreeValueMethod(HashTable *htable, void (*free_value)(void *));
size_t hashTableSize(HashTable *htable);
int HashTableContains(HashTable *htable, void *key);
HashTable *hashTableSet(HashTable *htable, void *key, void *value);
void *hashTableGet(HashTable *htable, void *key);
void *hashTableRemove(HashTable *htable, void *key);
HashTable *hashTableDel(HashTable *htable, void *key);
HashTable *hashTableClear(HashTable *htable);
void hashTableDestroy(HashTable *htable);
HashTableIter *hashTableIterator(HashTable *htable);

int hashTableIterHasNext(HashTableIter *iter);
void hashTableIterNext(HashTableIter *iter, void **key_ptr, void **value_ptr);
void hashTableIterDestroy(HashTableIter *iter);

#endif
