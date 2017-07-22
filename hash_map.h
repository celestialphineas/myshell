// Hash map in C
// -------------
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// This file implements a hash map
// The data structure implementation is relatively independent
// from the myshell project
#ifndef CELPHI_HASH_MAP_H
#define CELPHI_HASH_MAP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Exit codes
// ==========
// The exit codes definitions end with an extra underscore
#ifndef SUCCESS_
#define SUCCESS_                0
#endif
#ifndef MEM_ALLOC_ERR_
#define MEM_ALLOC_ERR_          2
#endif

// hash function type definition
// =============================
typedef int (*HashFunc)(const char*);

// HashEntry data structure
// =========================
// occupied         - Boolean value of the occupation status
// key              - Key string
// value            - Pointer to the data
// size             - Size of the data
typedef struct
{
    char occupied;
    char *key;
    void *value;
    unsigned size;
} HashEntry;

// Default initializer of the entry
// Note that this function does not actually allocate space
void init_entry(HashEntry *this_);
// Destructor of the entry
// Note that this function can destruct the entry data!
// Handle with care!
void destruct_entry(HashEntry *this_);

// HashMap data structure
// =======================
// size             - Size of the container
// entry_count      - Number of entries
// map              - The storage
// hash_function    - Function pointer to the hash function
typedef struct
{
    unsigned size;
    unsigned entry_count;
    HashEntry *map;
    HashFunc hash_function;
} HashMap;

// Public methods of HashMap
// ==========================
// Create a new hash map, constructor
// The size_scale shouble be an unsigned integer between 1 and 32
// This scale argument decides the initial size of the hash map
// The capacity (initial size) will set to be (1 << n) - 1
// Value size decides the key value size in each bucket
// HashFunc is the typename of a hash function
// This slot requires a hash function
HashMap *create_hash_map(unsigned size_scale, HashFunc);
// Destruct the hash map, destructor
// Default return value is NULL
// Notes that this function also destructs itself!
// Handle with care!
// Recommanded usage:
// hash_map = delete_hash_map(hash_map);
HashMap *delete_hash_map(HashMap *this_);
// The delete_entry is lazy.
// The behaviour is as follows:
//      Minus one on the entry_count.
//      Set the "occupied" flag off.
//      Delete the entry value, and flag the pointer NULL.
//      Reserve the key.
//      The position won't hold any longer if re-hashed.
// Return 1 if deletion succeeded
// Return 0 if failed
int delete_entry(HashMap *this_, const char *key_);
// Insert a key to the hash map
// Deep copy for both the key and the value
// The default behavior is in a overriding manner
int insert_entry(
    HashMap *this_, const char *key_, void *value_, unsigned size_);
// Rehash
// The rehashed hash map is twice as big
int rehash(HashMap *this_);
// Find
// The result is directly the hash entry, but read only.
const HashEntry *find_entry(HashMap *this_, const char *key_);
// Forced find
// Also find the unoccupied but dirty entries
const HashEntry *forced_find_entry(HashMap *this_, const char *key_);

// BKDR Hash Function
// This function does not return the remainder
// Only the hash value of the string is computed
// Return -1 if failed
int BKDR_hash(const char *str);

#endif