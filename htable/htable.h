#ifndef HTABLE_H
#define HTABLE_H

#include <stddef.h>

typedef struct HashTable HashTable;

HashTable *hashTableCreate(void *(*alloc)(size_t), void (*dealloc)(void *));
HashTable *hashTableSet(HashTable *htable, void *key, void *value);
void *hashTableGet(HashTable *htable, void *key);
void *hashTableRemove(HashTable *htable, void *key);
HashTable *hashTableDel(HashTable *htable, void *key);
HashTable *hashTableClear(HashTable *htable);
void hashTableDestroy(HashTable *htable);

#endif
