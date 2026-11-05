
#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int DEFAULT_PORT = 7;
const int BUF_SIZE = 1024;

bool send_all(int sock, const char* data, size_t length) {
    size_t total_sent = 0;

    while (total_sent < length) {
        ssize_t sent = send(
            sock,
            data + total_sent,
            length - total_sent,
            MSG_NOSIGNAL
        );

        if (sent < 0 && errno == EINTR) {
            continue;
        }

        if (sent <= 0) {
            return false;
        }

        total_sent += sent;
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <server_ip> [port]\n";
        return 1;
    }

    int port = DEFAULT_PORT;

    if (argc == 3) {
        try {
            std::string port_arg = argv[2];
            size_t pos = 0;

            int value = std::stoi(port_arg, &pos);

            if (pos != port_arg.length() ||
                value < 1 || value > 65535) {
                std::cerr << "Invalid port\n";
                return 1;
            }

            port = value;
        }
        catch (...) {
            std::cerr << "Invalid port\n";
            return 1;
        }
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        perror("socket");
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1],
                  &serv_addr.sin_addr) != 1) {
        std::cerr << "Invalid IP address\n";
        close(sock);
        return 1;
    }

    if (connect(sock, (sockaddr*)&serv_addr,
                sizeof(serv_addr)) == -1) {
        perror("connect");
        close(sock);
        return 1;
    }

    std::cout << "Connected to server "
              << argv[1] << ":" << port << "\n";

    std::string message;

    while (true) {
        std::cout << "Input message(Q to quit): ";

        if (!std::getline(std::cin, message)) {
            break;
        }

        if (message == "q" || message == "Q") {
            break;
        }

        if (message.empty()) {
            continue;
        }

        if (!send_all(sock, message.data(),
                      message.size())) {
            perror("send");
            break;
        }

        std::string response(message.size(), '\0');
        size_t total_received = 0;

        while (total_received < response.size()) {
            ssize_t received = recv(
                sock,
                &response[total_received],
                response.size() - total_received,
                0
            );

            if (received < 0 && errno == EINTR) {
                continue;
            }

            if (received <= 0) {
                std::cerr
                    << "Connection closed or receive error\n";
                close(sock);
                return 1;
            }

            total_received += received;
        }

        std::cout << "Message from server: "
                  << response << std::endl;
    }

    close(sock);
    return 0;
}

