#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

#define PORT 8080

int main() {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    
    // std::string bigMessage(10000, 'X');
    // send(sock, bigMessage.c_str(), bigMessage.size(), 0);

    // Uncomment below to test graceful shutdown
    std::string shutdownMsg = "shutdown";
    send(sock, shutdownMsg.c_str(), shutdownMsg.size(), 0);

    char buffer[1024] = {0};
    read(sock, buffer, sizeof(buffer));
    std::cout << "Server response: " << buffer << std::endl;

    close(sock);
    return 0;
}
