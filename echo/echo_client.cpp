#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 7;
const int BUF_SIZE = 1024;

void error_handling(const char *message) {
    std::cerr << message << std::endl;
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <IP>\n";
        return 1;
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) error_handling("socket() error");

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(PORT);

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");
    else
        std::cout << "Connected...........\n";

    char message[BUF_SIZE];
    while (true) {
        std::cout << "Input message(Q to quit): ";
        std::cin.getline(message, BUF_SIZE);

        if (std::strcmp(message, "q") == 0 || std::strcmp(message, "Q") == 0)
            break;

        int str_len = write(sock, message, std::strlen(message));

        int recv_len = 0;
        while (recv_len < str_len) {
            int recv_cnt = read(sock, &message[recv_len], BUF_SIZE - 1);
            if (recv_cnt == -1) error_handling("read() error!");
            recv_len += recv_cnt;
        }
        message[recv_len] = '\0';
        std::cout << "Message from server: " << message << std::endl;
    }
    
    close(sock);
    return 0;
}
