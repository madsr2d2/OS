#include "reverse_hash.h"
#include <openssl/sha.h>
#include <string.h>

// Function implementation for reverse_hash
uint64_t reverse_hash(uint8_t target_hash[32], uint64_t start, uint64_t end) {
    unsigned char sha256_result[SHA256_DIGEST_LENGTH];
    uint64_t i;
    for (i = start; i <= end; i++) {
        SHA256((unsigned char *)&i, sizeof(uint64_t), sha256_result); // sha256_result is in big-endian format so we don't need to reverse it.
        if (memcmp(sha256_result, target_hash, 32) == 0) {
            return i;
        }
    }
    return 0; // Not found
}
