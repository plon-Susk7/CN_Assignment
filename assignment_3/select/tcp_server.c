#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8080
#define MAX_CONNECTIONS 4000

void handle_client(int client_socket) {
    char client_message[2000];
    ssize_t read_size;

    while ((read_size = recv(client_socket, client_message, sizeof(client_message), 0) > 0)) {
        int n = atoi(client_message);
        if (n > 20) {
            n = 20;
        }

        long long int result = 1;
        for (int i = 1; i <= n; i++) {
            result *= i;
        }

        char response[2000];
        sprintf(response, "Factorial of %d is %lld", n, result);
        write(client_socket, response, strlen(response));
        memset(client_message, 0, sizeof(client_message));
    }

    if (read_size == 0) {
        puts("Client disconnected");
    } else if (read_size == -1) {
        perror("recv failed");
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket, c;
    struct sockaddr_in server, client;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_socket, MAX_CONNECTIONS);
    puts("Server listening");

    fd_set read_fds;
    int max_fd = server_socket;
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);

    while (1) {
        fd_set active_fds = read_fds;
        int num_ready_fds = select(max_fd + 1, &active_fds, NULL, NULL, NULL);

        for (int i = 0; i <= max_fd && num_ready_fds > 0; i++) {
            if (FD_ISSET(i, &active_fds)) {
                num_ready_fds--;

                if (i == server_socket) {
                    client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c);
                    FD_SET(client_socket, &read_fds);

                    if (client_socket > max_fd) {
                        max_fd = client_socket;
                    }

                    printf("Client connected\n");
                } else {
                    handle_client(i);
                    close(i);
                    FD_CLR(i, &read_fds);
                }
            }
        }
    }

    close(server_socket);

    return 0;
}
