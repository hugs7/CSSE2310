#ifndef stringstore_h

#include <stdlib.h>
#include <string.h>

#endif  // stringstore_h

typedef struct StringStore {
    char* key;
    char* value;
    struct StringStore* next;
} StringStore;

int stringstore_delete(StringStore* store, const char* key);

StringStore* stringstore_init(void) {
    StringStore* store = malloc(sizeof(StringStore));
    store->next = NULL;
    return store;
}

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
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    
    ptr->next = ss;
    
    //ss->next = store;
    //store = ss;
    return 1;
}

const char* stringstore_retrieve(StringStore* store, const char* key) {
    StringStore* ptr;
    for (ptr = store; ptr != NULL; ptr = ptr->next) {
        if (!strcmp(ptr->key, key)) {
            return ptr->value;
        }
    }
    
    return NULL;
}

int stringstore_delete(StringStore* store, const char* key) {
    StringStore* ptr;
    StringStore* prev;
    for (ptr = store, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next) {
        if (!strcmp(ptr->key, key)) {
            if (ptr->next != NULL) {    // if not the last key value pair
                if (prev == NULL) {     // if the first key value pair
                    ptr->key = ptr->next->key;
                    ptr->value = ptr->next->value;
                    //store = ptr->next;
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
