#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define PORT 8080
#define MAX_CONNECTIONS 4000
#define MAX_EVENTS 100

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

    int epoll_fd = epoll_create(MAX_EVENTS);
    struct epoll_event event;
    event.data.fd = server_socket;
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event);

    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == server_socket) {
                client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c);

                event.data.fd = client_socket;
                event.events = EPOLLIN;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event);

                printf("Client connected\n");
            } else {
                handle_client(events[i].data.fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
            }
        }
    }

    close(server_socket);

    return 0;
}
