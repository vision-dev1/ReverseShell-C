// Codes by vision

#include "crypto.h"

void encrypt_data(char *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        data[i] ^= ENCRYPTION_KEY;
    }
}

void decrypt_data(char *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        data[i] ^= ENCRYPTION_KEY;
    }
}
