#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/wait.h>
#include <signal.h>

#define PORT 8080

int server_sock;

// SIGCHLD handler to prevent zombie processes
void sigchld_handler(int) {
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

void handle_client(int client_sock) {
    char buffer[10240]; // Larger buffer for big messages
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRead;
    std::string message;

    // Read in chunks to handle very large client messages
    while ((bytesRead = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        message += buffer;

        // Check for shutdown command
        if (message.find("shutdown") != std::string::npos) {
            std::cout << "Shutdown command received. Closing server..." << std::endl;
            close(client_sock);
            close(server_sock);
            exit(0);
        }

        // If the client sent less than the buffer size, assume message end
        if (bytesRead < (ssize_t)sizeof(buffer) - 1) break;
    }

    std::cout << "Received: " << message << std::endl;
    write(client_sock, "Hello from C++ server", 22);
    close(client_sock);
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    
    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    // Handle zombie processes
    signal(SIGCHLD, sigchld_handler);

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // Accept loop
    while (true) {
        addr_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);

        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) { // Child process
            close(server_sock); // Child doesn’t need the main socket
            handle_client(client_sock);
            exit(0);
        }
        close(client_sock); // Parent doesn’t need the client socket
    }

    return 0;
}
