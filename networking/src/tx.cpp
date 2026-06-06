#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string> // Needed for std::to_string

#define DEST_IP "127.0.0.1" // Localhost
#define PORT 8080

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, DEST_IP, &dest_addr.sin_addr);

    int counter = 0;
    
    std::cout << "Starting UDP transmission..." << std::endl;

    while (true) {
        // Convert the integer counter to a string payload
        std::string payload = std::to_string(counter);
        
        sendto(sock, payload.c_str(), payload.length(), 0, 
              (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        
        // Use \r to return to the start of the line and std::flush to draw it.
        // The extra spaces ensure we overwrite leftover digits when going from 100 to 0.
        std::cout << "\rSent payload: " << counter << "   " << std::flush;
        
        // Increment and loop back to 0 after 100
        counter++;
        if (counter > 100) {
            counter = 0;
        }
        
        // Sleep for ~16ms to simulate 60fps
        usleep(16666); 
    }

    close(sock);
    return 0;
}
