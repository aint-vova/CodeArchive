#ifndef NUKA_HASH_H
#define NUKA_HASH_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Bucket {
	size_t keyHash;
	int value;
	struct Bucket *next;
} Bucket;

typedef struct HashTable {
	size_t size;
	size_t entriesCount;
	Bucket *buckets;
} HashTable;

size_t strHash(const char *str);

void htInit(HashTable *ht, size_t size);

bool htAdd(HashTable *ht, const char *key, int value);

bool htContains(HashTable *ht, const char *key);

bool htGet(HashTable *ht, const char *key, int *outValue);

bool htRemoveKey(HashTable *ht, const char *key);

void htPrintAll(HashTable *ht);

// htRemoveValue seems like a narrow-purpose thing, so lets wait with implementing that

#endif