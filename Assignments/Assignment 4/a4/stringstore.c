#ifndef stringstore_h

#include <stdlib.h>
#include <string.h>

#endif  // stringstore_h

typedef struct StringStore StringStore;

struct StringStore {
    char* key;
    char* value;
    StringStore* next;
};

int stringstore_delete(StringStore* store, const char* key);

// Create a new StringStore instance, and return a pointer to it
StringStore* stringstore_init(void) {
    StringStore* store = malloc(sizeof(StringStore));
    store->next = NULL;
    return store;
}

// Delete all memory associated with the given StringStore, and return NULL
StringStore* stringstore_free(StringStore* store) {
    StringStore* ptr;
    for (ptr = store; ptr != NULL; ptr = ptr->next) {
        free(ptr->key);
        free(ptr->value);
        free(ptr);
    }
    free(store);

    return NULL;
}

// Add the given 'key'/'value' pair to the StringStore 'store'.
// The 'key' and 'value' strings are copied with strdup() before being
// added to the database. Returns 1 on success, 0 on failure (e.g. if
// strdup() fails).
int stringstore_add(StringStore* store, const char* key, const char* value) { 
    stringstore_delete(store, key);  // clear key incase already exists
    StringStore* ss = stringstore_init();
    ss->key = strdup(key);
    ss->value = strdup(value);
    ss->next = NULL;
    if (ss->key == NULL || ss->value == NULL) {
        return 0;
    }

    StringStore* ptr = store;

    if (store->key == NULL || store->value == NULL) {
        store->key = ss->key;
        store->value = ss->value;
        store->next = NULL;
    } else {
        while (ptr != NULL) {
            if (ptr->next != NULL) {
             ptr = ptr->next;
            }
        }
    }
    
    ptr->next = ss;
    
    //ss->next = store;
    //store = ss;
    return 1;
}

// Attempt to retrieve the value associated with a particular 'key' in the
// StringStore 'store'.
// If the key exists in the database, return a const pointer to corresponding
// value string.
// If the key does not exist, return NULL
const char* stringstore_retrieve(StringStore* store, const char* key) {
    StringStore* ptr;
    if (store->key == NULL) {   // no elemnts in StringStore
        return NULL;
    }
    for (ptr = store; ptr != NULL; ptr = ptr->next) {
        if (!strcmp(ptr->key, key)) {
            return ptr->value;
        }
    }
    
    return NULL;
}

// Attempt to delete the key/value pair associated with a particular 'key' in
// the StringStore 'store'.
// If the key exists and deletion succeeds, return 1.
// Otherwise, return 0
int stringstore_delete(StringStore* store, const char* key) {
    StringStore* ptr;
    StringStore* prev;
    if (store->key == NULL || store->value == NULL) {
        return 0;
    }
    for (ptr = store, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next) {
        if (!strcmp(ptr->key, key)) {
            if (ptr->next != NULL) {    // if not the last key value pair
                if (prev == NULL) {     // if the first key value pair
                    ptr->key = ptr->next->key;
                    ptr->value = ptr->next->value;
                    ptr->next = ptr->next->next;
                } else {                // if not the first key value pair
                    prev->next = ptr->next;
                }
            } else if (prev != NULL) {  // if not the first key value pair
                prev->next = NULL;
            } else {                    // if only one element in StringStore
                store = NULL;
            }

            free(ptr->key);
            free(ptr->value);
            free(ptr);

            return 1;
        }
    }

    return 0;
}
