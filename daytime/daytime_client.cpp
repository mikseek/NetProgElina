#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 13;
const int BUF_SIZE = 1024;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <server_ip>\n";
        return 1;
    }

    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket() error");
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(PORT);

    const char* msg = "time";
    sendto(sock, msg, std::strlen(msg), 0, (sockaddr*)&serv_addr, sizeof(serv_addr));

    char buffer[BUF_SIZE];
    sockaddr_in from_addr{};
    socklen_t from_len = sizeof(from_addr);
    
    int str_len = recvfrom(sock, buffer, BUF_SIZE - 1, 0, (sockaddr*)&from_addr, &from_len);
    if (str_len == -1) {
        perror("recvfrom() error");
        close(sock);
        return 1;
    }
    
    buffer[str_len] = '\0';
    std::cout << "Server time: " << buffer << std::endl;

    close(sock);
    return 0;
}
