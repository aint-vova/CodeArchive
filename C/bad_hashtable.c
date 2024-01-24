#include "hash.h"

#include <miniaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t strHash(const char *str) {
	const int p = 31;
	const int m = 1e9 + 9;
	size_t hash = 0;
	size_t p_pow = 1;
	for (const char *c = str; *c != '\0'; ++c) {
		hash = (hash + (*c - 'a' + 1) * p_pow) % m;
		p_pow = (p_pow * p) % m;
	}
	return hash;
}

static bool htAddWithoutGrow(Bucket *buckets, size_t size, size_t hash, int value) {
	size_t hashModulo = hash % size;

	Bucket *curBucket = &buckets[hashModulo];

	while (curBucket->next != curBucket && curBucket->next != NULL) {
		if (curBucket->keyHash == hash) return false; //already exists, but do we need it for addWithoutGrow?
		//Cuz we are moving data that already was checked for duplicates. What if we do need this check?
		curBucket = curBucket->next;
	}

	bool atUnused = curBucket->next == NULL;

	if (!atUnused) {
		if (curBucket->keyHash == hash) return false;
		curBucket->next = malloc(sizeof(Bucket));
		curBucket = curBucket->next;
	}
	curBucket->keyHash = hash;
	curBucket->value = value;
	curBucket->next = curBucket;
	return true;
}

static void freeBucketArray(Bucket *bucketArray, size_t size) {
	// step 1 - free all children one by one
	for (size_t i = 0; i < size; ++i) {
		Bucket *rootBucket = &bucketArray[i];
		if (!rootBucket->next) continue; // its unused
		if (rootBucket->next != rootBucket) { // if its a root that has child
			Bucket *curBucket = rootBucket->next; // start from the child which exists, but not sure if its a deadend or no
			while (true) {
				// free child
				Bucket *nextBucket = curBucket->next;
				if (curBucket == nextBucket) {
					free(curBucket);
					break;
				}
				free(curBucket);
				curBucket = nextBucket;
			}
		}
		//free(rootBucket); // brother you can't, its a part of monolithically malloc-ed array
	}
	// step 2 - free entire array at once
	free(bucketArray);
	// also - it might be good if we'd maintain a POOL of buckets inside of hashtable, so that its just one free for all children
	// and make it sort of a dynamic array that resizes by factors of 2
}

static void htResizeAndRefill(HashTable *ht, size_t newSize) {
	// when we resizeAndRefill - our pairs get different locations but their->next gets inherited,
	// thus pointing to invalid locations. Damn, so much mistakes that i was completely unaware of...
	if (newSize == ht->size) return;
	Bucket *newBucketArray = calloc(newSize, sizeof(Bucket));
	//if calloc failed - buy more RAM :3
	//at this point, in newBucketArray we have a zeroed array of buckets,
	//and we just gotta fill them. Can we use normal functions?
	for (size_t i = 0; i < ht->size; ++i) {
		Bucket *curBucket = &ht->buckets[i];
		if (!curBucket->next) continue;
		while (true) {
			//add curBucket to the shit okay?
			htAddWithoutGrow(newBucketArray, newSize, curBucket->keyHash, curBucket->value); // brother what the fuckkkk
			// i have zero clue whether or not it will work seriously let me out of the cage give me back my pills bruh
			if (curBucket->next == curBucket) break; // break out if curBucket is a dead end
			curBucket = curBucket->next;
		}
	}
	//free(ht->buckets); // theres a leak cuz we arent freeing children of root bucket
	freeBucketArray(ht->buckets, ht->size); // now there shouldn't be any memory leak
	ht->buckets = newBucketArray;
	ht->size = newSize;
}

void htInit(HashTable *ht, size_t size) {
	ht->size = size;
	ht->buckets = calloc(size, sizeof(Bucket));
	ht->entriesCount = 0;
}

bool htAdd(HashTable *ht, const char *key, int value) {
	size_t hash = strHash(key);
	size_t hashModulo = hash % ht->size;
	printf("value %d is going into key %s, hash region is %lld!\n", value, key, hashModulo);
	Bucket *curBucket = &ht->buckets[hashModulo];
	
	while (curBucket->next != curBucket && curBucket->next != NULL) {
		if (curBucket->keyHash == hash) return false; //already exists
		curBucket = curBucket->next;
	}

	//now we are either at the unused (no check needed) or at the last one

	bool atUnused = curBucket->next == NULL;

	if (!atUnused) {
		if (curBucket->keyHash == hash) return false;
		curBucket->next = malloc(sizeof(Bucket));
		curBucket = curBucket->next;
	}
	curBucket->keyHash = hash;
	curBucket->value = value;
	curBucket->next = curBucket;
	++ht->entriesCount;
	float fillFactor = (float)ht->entriesCount / (float)ht->size;
	if (fillFactor >= 0.60f) {
		size_t oldSize = ht->size;
		size_t newSize = ht->size * 2;
		printf("fillFactor >= 0.60, expanded from %llu to %llu using htResizeAndRefill\n", oldSize, newSize);
		//htResizeAndRefill(ht, newSize);
	}
	return true;
}

bool htContains(HashTable *ht, const char *key) {
	size_t hash = strHash(key);
	size_t hashModulo = hash % ht->size;

	Bucket *curBucket = &ht->buckets[hashModulo];
	if (!curBucket->next) return false;
	while (true) {
		if (curBucket->keyHash == hash) return true;
		if (curBucket == curBucket->next) return false;
		curBucket = curBucket->next;
	}
}

bool htGet(HashTable *ht, const char *key, int *outValue) {
	size_t hash = strHash(key);
	size_t hashModulo = hash % ht->size;

	Bucket *curBucket = &ht->buckets[hashModulo];
	if (!curBucket->next) return false;
	while (true) {
		if (curBucket->keyHash == hash) {
			*outValue = curBucket->value;
			return true;
		}
		if (curBucket == curBucket->next) return false;
		curBucket = curBucket->next;
	}
}

bool htRemoveKey(HashTable *ht, const char *key) {
	size_t hash = strHash(key);
	size_t hashModulo = hash % ht->size;

	Bucket *curBucket = &ht->buckets[hashModulo];
	if (!curBucket->next) return false; //unused bucket, drop
	Bucket *previousBucket = NULL;
	while (true) {
		if (curBucket->keyHash == hash) {
			// its guaranteely not unused there btw (look upper)
			bool isRootBucket = curBucket == &ht->buckets[hashModulo];
			bool isLastBucket = curBucket->next == curBucket; // if its root then its still gonna indicate whether or not it has a tail
			if (isRootBucket) {
				if (isLastBucket) {
					// root (no back) but last (no continuation)
					curBucket->next = NULL; // mark it as unused
					goto successfullyRemoved;
				} else {
					// root, but has continuation (copy data from next to root)
					curBucket->keyHash = curBucket->next->keyHash;
					curBucket->value = curBucket->next->value;
					Bucket *danglingBucket = curBucket->next;
					curBucket->next = curBucket->next->next;
					free(danglingBucket); //TODO trying to free NULL there
					goto successfullyRemoved;
				}
			} else {
				if (isLastBucket) {
					// has back but not no continuation, then just rip it off and modify the back
					previousBucket->next = previousBucket;
					free(curBucket);
					goto successfullyRemoved;
				}
				else {
					// has back and continuation, relink continuation to back
					previousBucket->next = curBucket->next;
					free(curBucket);
					goto successfullyRemoved;
				}
			}
		}
		if (curBucket == curBucket->next) return false;
		previousBucket = curBucket;
		curBucket = curBucket->next;
	}
	//shouldn't be possibly reached...
	//all of this code REALLY begs for a refactor, but thats for later xD
	successfullyRemoved:
	--ht->entriesCount;
	float fillFactor = (float)ht->entriesCount / (float)ht->size;
	if (fillFactor <= 0.40f) {
		size_t oldSize = ht->size;
		size_t newSize = ht->size / 2;
		printf("fillFactor <= 0.40, shrinked from %llu to %llu using htResizeAndRefill\n", oldSize, newSize);
		//htResizeAndRefill(ht, newSize);
	}
	return true;
}

void htPrintAll(HashTable *ht) {
	for (size_t i = 0; i < ht->size; ++i) {
		Bucket *curBucket = &ht->buckets[i];
		printf("\n--- Exploring hash zone: %llu!\n", i);
		if (!curBucket->next) {
			printf("unused.\n");
			continue;
		}
		//its not an unused, guaranteed.
		while (true) {
			printf("Key: %llu, value: %d\n", curBucket->keyHash, curBucket->value);
			if (curBucket->next == curBucket) break;
			curBucket = curBucket->next;
		}
	}
}
