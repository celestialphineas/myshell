// Implementation of operations defined in a HashMap
#include "hash_map.h"

void init_entry(HashEntry *this_)
{
    if(!this_) return;
    this_->occupied = 0;
    this_->key = NULL;
    this_->size = 0;
    this_->value = NULL;
    return;
}

void destruct_entry(HashEntry *this_)
{
    if(!this_) return;
    this_->occupied = 0;
    this_->size = 0;
    if(this_->key) free(this_->key);
    this_->key = NULL;
    if(this_->value) free(this_->value);
    this_->value = NULL;
    return;
}

HashMap *create_hash_map(unsigned size_scale, HashFunc hash_function_)
{
    int i;
    HashMap *new_hash_map = (HashMap*)malloc(sizeof(HashMap));
    // Too large or small hash map!
    if(size_scale < 1 || size_scale > 32) return NULL;
    // (1 << n) - 1 is "almost" prime, for most n
    new_hash_map->size = (1<<size_scale) - 1;
    // Empty entry
    new_hash_map->entry_count = 0;
    // Allocate space
    new_hash_map->map = 
        (HashEntry*)malloc(new_hash_map->size * sizeof(HashEntry));
    if(!new_hash_map->map) return NULL;
    // Call the "constructors"
    for(i = 0; i < new_hash_map->size; i++)
    {
        init_entry(new_hash_map->map + i);
    }
    new_hash_map->hash_function = hash_function_;
    return new_hash_map;
}

HashMap *delete_hash_map(HashMap *this_)
{
    int i;
    if(!this_) return NULL;
    if(!this_->map)
    {
        for(i = 0; i < this_->size; i++)
        {
            destruct_entry(this_->map + i);
        }
        free(this_->map);
    }
    this_->map = NULL;
    free(this_);
    return NULL;
}

int delete_entry(HashMap *this_, const char *key_)
{
    int i;
    int hash_value;
    if(!this_) return 0;
    // Meets NULL
    if(!key_) return 0;
    hash_value = this_->hash_function(key_);
    // Meets hash failure
    if(hash_value == -1) return 0;
    // There is no special reason to set the upper bound as this_->size/2
    // Just in case for an infinite loop
    for(i = 0; i < this_->size; i++)
    {
        // Square detection
        int index = (hash_value + i * i) % this_->size;
        // Not found, meets a virgin
        if(!this_->map[index].occupied && !this_->map[index].key)
            return 0;
        // Found
        if(this_->map[index].occupied && !strcmp(this_->map[index].key, key_))
        {
            // Minus one on the entry_count
            this_->entry_count--;
            // Set the "occupied" flag off
            this_->map[index].occupied = 0;
            // Delete the entry value, and flag the pointer to NULL
            if(this_->map[index].value) free(this_->map[index].value);
            this_->map[index].value = NULL;
            // Set the size 0
            this_->map[index].size = 0;
            // The key is reserved
            // Return success
            return 1;
        }
    }
    return 0;
}

int insert_entry(
    HashMap *this_, const char *key_, void *value_, unsigned size_)
{
    int i;
    int hash_value;
    int rehash_success;
    if(!this_) return 0;
    // Meets NULL
    if(!key_) return 0;
    hash_value = this_->hash_function(key_);
    // Meets hash failure
    if(hash_value == -1) return 0;
    // If the table is half full, rehash
    if((this_->entry_count + 1)*2 > this_->size)
    {
        rehash_success = rehash(this_);
        // Return failure if rehash fails
        if(!rehash_success) return 0;
    }
    for(i = 0; i < this_->size; i++)
    {
        // Square detection
        int index = (hash_value + i * i) % this_->size;
        if(!this_->map[index].occupied)
        {
            // Not a virgin, free the space
            if(this_->map[index].key)
            {
                free(this_->map[index].key);
                this_->map[index].key = NULL;
            }
            if(this_->map[index].value)
            {
                free(this_->map[index].value);
                this_->map[index].value = NULL;
            }
            // Plus one on the entry_count
            this_->entry_count++;
            // Set the "occupied" flag on
            this_->map[index].occupied = 1;
            // Allocate space and copy the key
            this_->map[index].key
                = (char*)malloc((strlen(key_) + 1) * sizeof(char));
            if(!this_->map[index].key) return 0;
            strcpy(this_->map[index].key, key_);
            // Allocate space and copy the value
            this_->map[index].value = malloc(size_);
            if(!this_->map[index].value) return 0;
            memcpy(this_->map[index].value, value_, size_);
            this_->map[index].size = size_;
            return 1;
        }
        // Indentical keys, override the old
        else if(!strcmp(this_->map[index].key, key_))
        {
            if(this_->map[index].value)
                free(this_->map[index].value);
            this_->map[index].value = NULL;
            this_->map[index].value = malloc(size_);
            if(!this_->map[index].value) return 0;
            memcpy(this_->map[index].value, value_, size_);
            this_->map[index].size = size_;
            return 1;
        }
    }
    return 0;
}

int rehash(HashMap *this_)
{
    int i, j;
    unsigned old_size, new_size;
    HashEntry *temp_map;
    // Meets NULL
    if(!this_) return 0;
    // Get the old size
    old_size = this_->size;
    // Illegal old size
    if(old_size >= (unsigned)(-1)) return 0;
    // Calculate the new size
    new_size = ((old_size + 1) << 1) -1;
    // Intialization of the temp map
    temp_map = (HashEntry*)malloc(new_size * sizeof(HashEntry));
    if(!temp_map) return 0;
    for(i = 0; i < new_size; i++)
    {
        init_entry(temp_map + i);
    }
    // Handle the old map and push the pairs into the new
    for(i = 0; i < old_size; i++)
    {
        // For all that are occupied, shallow copy.
        if(this_->map[i].occupied)
        {
            int success_flag = 0;
            char *key_ = this_->map[i].key;
            void *value_ = this_->map[i].value;
            unsigned size_ = this_->map[i].size;
            int hash_value = this_->hash_function(key_);
            for(j = 0; j < new_size; j++)
            {
                int index = (hash_value + j * j) % new_size;
                if(!temp_map[index].occupied)
                {
                    // Set the "occupied" flag on
                    temp_map[index].occupied = 1;
                    // Set the key, value and size
                    temp_map[index].key = key_;
                    temp_map[index].value = value_;
                    temp_map[index].size = size_;
                    success_flag = 1;
                    break;
                }
            }
            if(!success_flag) return 0;
        }
        // For the else, free space
        else
        {
            // Free the space
            if(this_->map[i].key)
            {
                free(this_->map[i].key);
                this_->map[i].key = NULL;
            }
            if(this_->map[i].value)
            {
                free(this_->map[i].value);
                this_->map[i].value = NULL;
            }
        }
    }
    // Replace the old one.
    free(this_->map);
    this_->map = temp_map;
    this_->size = new_size;
    return 1;
}

const HashEntry *find_entry(HashMap *this_, const char *key_)
{
    int i;
    int hash_value;
    // Meets NULL arguments
    if(!this_) return NULL;
    if(!key_)   return NULL;
    // Compute hash value
    hash_value = this_->hash_function(key_);
    // Meets hash failure
    if(hash_value == -1) return 0;
    for(i = 0; i < this_->size; i++)
    {
        // Square detection
        int index = (hash_value + i * i) % this_->size;
        // Not found, meets a virgin
        if(!this_->map[index].occupied && !this_->map[index].key)
            return 0;
        // Found
        if(this_->map[index].occupied && !strcmp(this_->map[index].key, key_))
            return this_->map + index;
    }
    return NULL;
}

const HashEntry *forced_find_entry(HashMap *this_, const char *key_)
{
    int i;
    int hash_value;
    // Meets NULL arguments
    if(!this_) return NULL;
    if(!key_)   return NULL;
    // Compute hash value
    hash_value = this_->hash_function(key_);
    // Meets hash failure
    if(hash_value == -1) return 0;
    for(i = 0; i < this_->size; i++)
    {
        // Square detection
        int index = (hash_value + i * i) % this_->size;
        // Not found, meets a virgin
        if(!this_->map[index].occupied && !this_->map[index].key)
            return 0;
        // Found
        if(!strcmp(this_->map[index].key, key_))
            return this_->map + index;
    }
    return NULL;
}

int BKDR_hash(const char *str)
{
    // 31 131 1313 13131 131313
    int magic = 131;
    int hash = 0;
    // Set upper bound to prevent infinite loop
    int upper = 64;
    unsigned char *ch = (unsigned char*)str;

    // If the string points NULL
    if(!str) return -1;
    // Compute the hash value
    while(*ch && (char*)ch - (char*)str < upper)
    {
        hash = hash * magic + (*ch++);
    }
    // Make sure that the return value is positive
    return hash & 0x7fffffff;
}
