#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CONNECTIONS 4000

void *connection_handler(void *socket_desc) {
    int client_socket = *(int *)socket_desc;
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

    free(socket_desc);
    close(client_socket);
    return NULL;
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

    c = sizeof(struct sockaddr_in);

    pthread_t thread_id;

    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c))) {
        int *new_sock = malloc(1);
        *new_sock = client_socket;

        if (pthread_create(&thread_id, NULL, connection_handler, (void *)new_sock) < 0) {
            perror("Could not create thread");
            return 1;
        }

        pthread_detach(thread_id);
    }

    if (client_socket < 0) {
        perror("Accept failed");
        return 1;
    }

    close(server_socket);

    return 0;
}
