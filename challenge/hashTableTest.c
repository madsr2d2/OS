#include "hashTable.h"
#include <stdio.h>

int main() {
    HashTable *hashTable = createHashTable();
    if (!hashTable) {
        return -1;
    }

    insert(hashTable, (uint8_t*)"name", 123);
    insert(hashTable, (uint8_t*)"age", 25);
    insert(hashTable, (uint8_t*)"address", 456);

    uint64_t value;
    if (search(hashTable, (uint8_t*)"name", &value) == 0) {
        printf("Value of 'name': %lu\n", value);
    }
    if (search(hashTable, (uint8_t*)"age", &value) == 0) {
        printf("Value of 'age': %lu\n", value);
    }
    if (search(hashTable, (uint8_t*)"address", &value) == 0) {
        printf("Value of 'address': %lu\n", value);
    }
    if (search(hashTable, (uint8_t*)"phone", &value) == 0) {
        printf("Value of 'phone': %lu\n", value);
    }

    freeHashTable(hashTable);
    return 0;
}
