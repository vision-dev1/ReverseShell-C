// Codes by vision

#ifndef CRYPTO_H
#define CRYPTO_H

#include <stddef.h>

#define BUFFER_SIZE 4096
#define ENCRYPTION_KEY 0xAA

void encrypt_data(char *data, size_t length);
void decrypt_data(char *data, size_t length);

#endif
