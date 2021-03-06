#include "hashtable.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define EXPAND_THRESHOLD 1
#define SHRINK_THRESHOLD 0.1

#define MIN_TABLE_SIZE 8

typedef struct TableEntry {
	void *key;
	void *value;
	struct TableEntry *next;
} TableEntry;

typedef struct Table {
	TableEntry **entries;
	size_t count;
	/* size always equals to 2^n */
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

struct HashTableIter {
	HashTable *table;
	TableEntry *next;
	size_t current_table_idx;
	size_t current_index;
	void (*dealloc)(void *);
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

size_t (*getHashMethod(HashTable *htable))(void *) { return htable->hash; }

void setHashMethod(HashTable *htable, size_t (*hash)(void *))
{
	htable->hash = hash;
}

int (*getCompareMethod(HashTable *htable))(void *, void *)
{
	return htable->compare;
}

void setCompareMethod(HashTable *htable, int (*compare)(void *, void *))
{
	htable->compare = compare;
}

void (*getFreeKeyMethod(HashTable *htable))(void *) { return htable->free_key; }

void setFreeKeyMethod(HashTable *htable, void (*free_key)(void *))
{
	htable->free_key = free_key;
}

void (*getFreeValueMethod(HashTable *htable))(void *)
{
	return htable->free_value;
}

void setFreeValueMethod(HashTable *htable, void (*free_value)(void *))
{
	htable->free_value = free_value;
}

size_t hashTableSize(HashTable *htable)
{
	Table *table1 = htable->tables;
	if (table1->entries == NULL) {
		return 0;
	}

	if (htable->rehash_idx == -1) {
		return table1->count;
	}

	return table1->count + htable->tables[1].count;
}

static HashTable *hashTableInit(HashTable *htable)
{
	Table *table = htable->tables;
	if (table->entries != NULL) {
		return htable;
	}

	table->entries = htable->alloc(sizeof(Table *) * MIN_TABLE_SIZE);
	memset(table->entries, 0, sizeof(Table *) * MIN_TABLE_SIZE);
	return htable;
}

static HashTable *hashTableGetIndex(HashTable *htable, void *key,
				    size_t *table_idx, size_t *index)
{
	size_t hash = htable->hash(key);
	*index = hash & (htable->tables[0].size - 1);
	if (*index < htable->rehash_idx) {
		*table_idx = 1;
		*index = hash & (htable->tables[1].size - 1);
	} else {
		*table_idx = 0;
	}

	return htable;
}

static HashTable *hashTableResize(HashTable *htable, size_t size)
{
	Table *table2 = htable->tables + 1;
	table2->entries = htable->alloc(sizeof(TableEntry *) * size);
	memset(table2->entries, 0, sizeof(TableEntry *) * size);
	table2->count = 0;
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

static HashTable *hashTableCheckThreShold(HashTable *htable)
{
	if (htable->rehash_idx != -1) {
		hashTableReHash(htable);
		return htable;
	}

	Table *table1 = htable->tables;
	Table *table2 = htable->tables + 1;

	if (table1->size == MIN_TABLE_SIZE) {
		return htable;
	}

	double threshold = (double)table1->count / table1->size;

	if (threshold > SHRINK_THRESHOLD && threshold < EXPAND_THRESHOLD) {
		return htable;
	}

	size_t size = 2;
	while (size < table1->count) {
		size <<= 1;
	}

	if (threshold > EXPAND_THRESHOLD) {
		size <<= 1;
	}

	size = size < MIN_TABLE_SIZE ? MIN_TABLE_SIZE : size;

	hashTableResize(htable, size);
	hashTableReHash(htable);

	return htable;
}

void hashTableSet(HashTable *htable, void *key, void *value)
{
	if (htable->tables[0].entries == NULL) {
		hashTableInit(htable);
	}

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
		++htable->tables[table_idx].count;
	}

	hashTableCheckThreShold(htable);
}

void *hashTableGet(HashTable *htable, void *key)
{
	if (htable->tables[0].entries == NULL) {
		return NULL;
	}

	size_t index;
	size_t table_idx;
	hashTableGetIndex(htable, key, &table_idx, &index);

	TableEntry *entry = htable->tables[table_idx].entries[index];
	while (entry != NULL) {
		if (htable->compare(entry->key, key) == 0) {
			return entry->value;
		}
	}

	return NULL;
}

int HashTableContains(HashTable *htable, void *key)
{
	return hashTableGet(htable, key) != NULL;
}

void *hashTableRemove(HashTable *htable, void *key)
{
	size_t index;
	size_t table_idx;
	hashTableGetIndex(htable, key, &table_idx, &index);

	TableEntry **entry_ptr = htable->tables[table_idx].entries + index;
	TableEntry *entry = *entry_ptr;
	void *value = NULL;
	while (entry != NULL) {
		if (htable->compare(entry->key, key) == 0) {
			break;
		}

		entry_ptr = &entry->next;
		entry = *entry_ptr;
	}

	if (entry != NULL) {
		value = entry->value;
		if (htable->free_key != NULL) {
			htable->free_key(entry->key);
		}

		*entry_ptr = entry->next;
		htable->dealloc(entry);
	}

	hashTableCheckThreShold(htable);

	return value;
}

void hashTableDel(HashTable *htable, void *key)
{
	hashTableRemove(htable, key);
}

HashTable *hashTableDestroyEntryList(HashTable *htable, TableEntry *head)
{
	TableEntry *tmp = NULL;
	while (head != NULL) {
		tmp = head;
		head = head->next;

		if (htable->free_key != NULL) {
			htable->free_key(tmp->key);
		}

		if (htable->free_value != NULL) {
			htable->free_value(tmp->value);
		}

		htable->dealloc(tmp);
	}

	return htable;
}

void hashTableClear(HashTable *htable)
{
	Table *table1 = htable->tables;
	Table *table2 = htable->tables + 1;

	int i;
	if (htable->rehash_idx != -1) {
		for (i = htable->rehash_idx; i < table1->size; ++i) {
			hashTableDestroyEntryList(htable, table1->entries[i]);
		}

		for (i = 0; i < table2->size; ++i) {
			hashTableDestroyEntryList(htable, table2->entries[i]);
		}

		htable->dealloc(table2->entries);
	} else {
		for (i = 0; i < table1->size; ++i) {
			hashTableDestroyEntryList(htable, table1->entries[i]);
		}
	}

	htable->dealloc(table1->entries);

	table1->entries = NULL;
	table2->entries = NULL;
	htable->rehash_idx = -1;
}

void hashTableDestroy(HashTable *htable)
{
	hashTableClear(htable);
	htable->dealloc(htable);
}

HashTableIter *hashTableIterator(HashTable *htable)
{
	HashTableIter *iter = htable->alloc(sizeof(HashTableIter));
	iter->table = htable;
	iter->current_table_idx = 0;
	iter->current_index = htable->rehash_idx != -1 ? htable->rehash_idx : 0;
	iter->next = htable->tables[0].entries[iter->current_index];
	iter->dealloc = htable->dealloc;
	return iter;
}

int hashTableIterHasNext(HashTableIter *iter) { return iter->next != NULL; }

void hashTableIterNext(HashTableIter *iter, void **key_ptr, void **value_ptr)
{
	*key_ptr = iter->next->key;
	*value_ptr = iter->next->value;

	iter->next = iter->next->next;
	if (iter->next != NULL) {
		return;
	}

	Table *table1 = iter->table->tables;
	Table *table2 = iter->table->tables + 1;

	int isRehashing = iter->table->rehash_idx != -1;

	while (iter->next == NULL) {
		if (!isRehashing && iter->current_index == table1->size - 1 ||
		    isRehashing && iter->current_table_idx == 1 &&
			iter->current_index == table2->size - 1) {
			break;
		}

		if (isRehashing && iter->current_table_idx == 0 &&
		    iter->current_index == table1->size - 1) {
			iter->current_table_idx = 1;
			iter->current_index = 0;
		} else {
			++iter->current_index;
		}

		iter->next = iter->table->tables[iter->current_table_idx]
				 .entries[iter->current_index];
	}
}

void hashTableIterDestroy(HashTableIter *iter) { iter->dealloc(iter); }
