#include <iostream>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 44214;
const int BUF_SIZE = 1024;

bool send_all(int sock, const char* data, size_t length) {
    size_t sent = 0;

    while (sent < length) {
        ssize_t n = send(sock, data + sent,
                         length - sent, MSG_NOSIGNAL);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) return false;
        sent += n;
    }
    return true;
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sock == -1) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(server_sock);
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
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        std::cout << "Client connected\n";

        while (true) {
            ssize_t received = recv(
                client_sock, buffer, sizeof(buffer), 0
            );

            if (received == 0) {
                std::cout << "Client disconnected\n";
                break;
            }

            if (received < 0) {
                if (errno == EINTR) continue;
                perror("recv");
                break;
            }

            if (!send_all(client_sock, buffer, received)) {
                std::cerr << "Send error\n";
                break;
            }
        }

        close(client_sock);
    }

    close(server_sock);
    return 0;
}
