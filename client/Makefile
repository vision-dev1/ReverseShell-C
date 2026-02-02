# Codes by vision

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = 
TARGET = client
CRYPTO_DIR = ../common
CRYPTO_SRC = $(CRYPTO_DIR)/crypto.c
CRYPTO_OBJ = crypto.o
SRC = client.c
OBJ = client.o

all: $(TARGET)

$(TARGET): $(OBJ) $(CRYPTO_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(CRYPTO_OBJ) $(LDFLAGS)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

$(CRYPTO_OBJ): $(CRYPTO_SRC)
	$(CC) $(CFLAGS) -c $(CRYPTO_SRC)

debug: CFLAGS += -g -DDEBUG
debug: clean all

release: CFLAGS += -O2
release: clean all

clean:
	rm -f $(TARGET) $(OBJ) $(CRYPTO_OBJ)

.PHONY: all debug release clean
