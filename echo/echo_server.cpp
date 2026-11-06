#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 44214;
const int BUF_SIZE = 1024;

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sock == -1) {
        perror("socket");
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (sockaddr*)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 5) == -1) {
        perror("listen");
        close(server_sock);
        return 1;
    }

    std::cout << "Echo server started on port "
              << PORT << std::endl;

    char buffer[BUF_SIZE];

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_sock = accept(
            server_sock,
            (sockaddr*)&client_addr,
            &client_len
        );

        if (client_sock == -1) {
            perror("accept");
            continue;
        }

        std::cout << "Client connected" << std::endl;

        while (true) {
            ssize_t received = recv(
                client_sock, buffer, BUF_SIZE, 0
            );

            if (received == 0) {
                std::cout << "Client disconnected"
                          << std::endl;
                break;
            }

            if (received == -1) {
                perror("recv");
                break;
            }

            ssize_t total_sent = 0;

            while (total_sent < received) {
                ssize_t sent = send(
                    client_sock,
                    buffer + total_sent,
                    received - total_sent,
                    MSG_NOSIGNAL
                );

                if (sent <= 0) {
                    perror("send");
                    break;
                }

                total_sent += sent;
            }

            if (total_sent != received) {
                break;
            }
        }

        close(client_sock);
    }

    close(server_sock);
    return 0;
}
