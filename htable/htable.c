#include "pair.h"
#include "htable.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_EXPAND_THRESHOLD 1
#define DEFAULT_SHRINK_THRESHOLD 0.1

typedef struct TableEntry {
	void *key;
	void *value;
	struct TableEntry *next;
} TableEntry;

typedef struct Table {
	TableEntry **entries;
	size_t used;
	/* size is always 2^n */
	size_t size;
} Table;

struct HashTable {
	Table tables[2];
	size_t rehash_idx;
	double expand_threshold;
	double shrink_threshold;

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
	htable->expand_threshold = DEFAULT_EXPAND_THRESHOLD;
	htable->shrink_threshold = DEFAULT_SHRINK_THRESHOLD;
	return htable;
}

static HashTable *hashTableResize(HashTable *htable, size_t size)
{
	Table *table2 = htable->tables + 1;
	table2->entries = htable->alloc(sizeof(TableEntry *) * size);
	memset(table2->entries, 0, sizeof(TableEntry *) * size);
	table2->used = 0;
	table2->size = size;
	return htable;
}

static HashTable *hashTableReHash(HashTable *htable)
{
	Table *table1 = htable->tables;
	Table *table2 = htable->tables + 1;

	assert(table1->entries != NULL);
	assert(table2->entries != NULL);

	TableEntry *entry = table1->entries[htable->rehash_idx];
	while (entry == NULL && htable->rehash_idx < table1->size) {
		++htable->rehash_idx;
		entry = table1->entries[htable->rehash_idx];
	}

	if (htable->rehash_idx == table1->size) {
		htable->rehash_idx = -1;
		htable->dealloc(table1->entries);
		memcpy(table1, table2, sizeof(Table));
		memset(table2, 0, sizeof(Table));
		return htable;
	}

	table1->entries[htable->rehash_idx] = NULL;

	TableEntry *tmp = NULL;
	size_t index;
	while (entry != NULL) {
		index = htable->hash(entry->key) & (table2->size - 1);
		tmp = entry;
		entry = entry->next;
		tmp->next = table2->entries[index];
		table2->entries[index] = tmp;
	}

	++htable->rehash_idx;

	return htable;
}

HashTable *hashTableSet(HashTable *htable, void *key, void *value)
{
	size_t table_idx = 0;
	if (htable->rehash_idx != -1) {
		table_idx = 1;
	}

	size_t index = htable->hash(key) & (htable->tables[table_idx].size - 1);
	TableEntry *entry = htable->tables[table_idx].entries[index];
	while (entry != NULL) {
		if (htable->compare(entry->key, key) == 0) {
			if (htable->free_value != NULL) {
				htable->free_value(entry->value);
			}

			entry->value = value;
			break;
		}

		entry = entry->next;
	}

	if (entry == NULL) {
		entry = htable->alloc(sizeof(TableEntry));
		entry->key = key;
		entry->value = value;
		entry->next = htable->tables[table_idx].entries[index];
		htable->tables[table_idx].entries[index] = entry;
		++htable->tables[table_idx].used;
	}

	// TODO check threshold

	return htable;
}

void *hashTableGet(HashTable *htable, void *key)
{
	size_t hash = htable->hash(key);
	size_t index = hash & (htable->tables[0].size - 1);
	TableEntry *entry = NULL;
	if (htable->rehash_idx <= index) {
		entry = htable->tables[0].entries[index];
	} else {
		index = hash & (htable->tables[1].size - 1);
		entry = htable->tables[1].entries[index];
	}

	while (entry != NULL) {
		if (htable->compare(entry->key, key) == 0) {
			return entry->value;
		}
	}

	return NULL;
}

void *hashTableRemove(HashTable *htable, void *key) {}

HashTable *hashTableDel(HashTable *htable, void *key) {}

HashTable *hashTableClear(HashTable *htable) { return htable; }

void hashTableDestroy(HashTable *htable)
{
	hashTableClear(htable);
	htable->dealloc(htable);
}
