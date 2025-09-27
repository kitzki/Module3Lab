#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>

#define PORT 8080
#define TARGET_IP "127.0.0.1"
#define CONNECTIONS 500 // number of simultaneous connections

int main() {
    std::vector<int> sockets;

    for (int i = 0; i < CONNECTIONS; i++) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Socket creation failed");
            continue;
        }

        struct sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        inet_pton(AF_INET, TARGET_IP, &server_addr.sin_addr);

        if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            close(sock);
            continue;
        }

        std::cout << "Opened connection #" << i + 1 << std::endl;
        sockets.push_back(sock); // keep socket open
    }

    std::cout << "All connections opened. Press Enter to close them." << std::endl;
    std::cin.get();

    for (int sock : sockets) {
        close(sock);
    }

    return 0;
}
