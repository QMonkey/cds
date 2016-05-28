#include "pair.h"
#include "htable.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct TableEntry {
	void *key;
	void *value;
	struct TableEntry *next;
} TableEntry;

typedef struct Table {
	TableEntry **entries;
	size_t used;
	size_t size;
} Table;

struct HashTable {
	Table tables[2];
	size_t rehash_idx;

	void *(*alloc)(size_t);
	void (*dealloc)(void *);
	size_t (*hash)(void *);
	int (*compare)(void *, void *);
	void (*free_key)(void *);
	void (*free_value)(void *);
};

HashTable *hashTableCreate(void *(*alloc)(size_t), void (*dealloc)(void *))
{
	HashTable *htable = alloc(sizeof(HashTable));
	memset(htable, 0, sizeof(HashTable));
	htable->alloc = alloc;
	htable->dealloc = dealloc;
	htable->rehash_idx = -1;
	return htable;
}

static HashTable *tableResize(HashTable *htable, size_t size) { return htable; }

HashTable *hashTableSet(HashTable *htable, void *key, void *value)
{
	return htable;
}

void *hashTableGet(HashTable *htable, void *key) {}

void *hashTableRemove(HashTable *htable, void *key) {}

HashTable *hashTableDel(HashTable *htable, void *key) {}

HashTable *hashTableClear(HashTable *htable) { return htable; }

void hashTableDestroy(HashTable *htable)
{
	hashTableClear(htable);
	htable->dealloc(htable);
}
