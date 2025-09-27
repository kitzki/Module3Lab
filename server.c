#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8080

void handle_client(int client_sock) {
    char buffer[1024] = {0};

    int bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0'; // null-terminate
        printf("Received: %s\n", buffer);
        write(client_sock, "Hello from server", 17);
    }

    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Reap zombie processes automatically
    signal(SIGCHLD, SIG_IGN);

    // Create socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept loop
    while (1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) {
            // Child process
            close(server_sock); // child doesn’t need listener socket
            handle_client(client_sock);
            exit(0);
        }
        // Parent process
        close(client_sock);
    }

    close(server_sock);
    return 0;
}
