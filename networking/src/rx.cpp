#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sock, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    std::cout << "Listening for media on port " << PORT << "..." << std::endl;

    char buffer[BUFFER_SIZE];
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (true) {
        int n = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, 
                        (struct sockaddr *)&client_addr, &client_len);
        if (n > 0) {
            buffer[n] = '\0'; // Null-terminate the string so it prints correctly
            
            // Use \r and flush to update the line cleanly
            std::cout << "\rReceived payload: " << buffer << "   " << std::flush;
        }
    }

    close(sock);
    return 0;
}
