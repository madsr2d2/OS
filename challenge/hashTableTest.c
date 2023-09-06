#include "hashTable.h"
#include <stdio.h>

int main() {
    HashTable *hashTable = createHashTable();
    if (!hashTable) {
        return -1;
    }


    uint8_t key1[] = {'d', 'h', 'N', 'e', 'n', 'y', 'w', 'w', 'r', 'i', 'f', 'Z', 'Y', 'U', '0', 'g', 'R', 'v', '1', 'T', 'b', '1', 'r', 'q', 'E', 'K', 'f', 'b', '9', 'D', 'M'};
    uint8_t key2[] = {'C', '0', 'C', 'F', '9', 'I', '9', '3', 'z', 'Y', 'T', 'P', 'i', 'M', 'u', 'C', 'p', 'i', 'c', 'n', 'i', 'G', 'W', 'W', 'W', 'S', 'W', 'z', 'o', 'F', '4'};
    uint8_t key3[] = {'Z', 'Y', 'r', 'm', '0', 'S', 'j', '9', '8', '4', 'U', 'K', 'Z', 'G', 'M', 'y', 'K', 'i', 'Q', 'F', 'y', 'm', 'h', 'Z', 'f', 'F', 'S', '8', 'S', 'O', 'V'};
    uint8_t key4[] = {'D', 'O', 'q', 'N', 'w', 'j', 'A', 'l', 'X', 'z', 'I', 'V', 'V', 'J', 'p', 'b', 'R', 'g', 'Y', '2', '4', 'l', 'M', 'u', 'D', 'Q', '0', 'B', 'z', 'S', '1'};
    uint8_t *key5 = key1;



    insert(hashTable, key1, 9974818013591297781ULL);
    insert(hashTable, key2, 7583640683233364723ULL);
    insert(hashTable, key5, 13688362743156890176ULL);



    uint64_t value;
    if (search(hashTable, key1, &value) == 0) {
        printf("Value of key1: %lu\n", value);
    }
    if (search(hashTable, key2, &value) == 0) {
        printf("Value of key2: %lu\n", value);
    }
    if (search(hashTable, key5, &value) == 0) {
        printf("Value of key5: %lu\n", value);
    }
    if (search(hashTable, key4, &value) == 0) {
        printf("Value of key4: %lu\n", value);
    }



    freeHashTable(hashTable);
    return 0;
}
