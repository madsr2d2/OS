#ifndef REVERSE_HASH_H
#define REVERSE_HASH_H

#include <stdint.h>
#include <openssl/sha.h>
#include <string.h>

// Function prototype for reverse_hash
uint64_t reverse_hash(uint8_t target_hash[32], uint64_t start, uint64_t end);

#endif // REVERSE_HASH_H
