#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t output_mutex;

int client_count = 0;

struct thread_args {
    int client_fd;
};

long long int factorial(int n) {
    if (n == 0) {
        return 1;
    }

    if (n > 20) n = 20;

    long long int result = 1;

    for (int i = 1; i <= n; i++) {
        result *= i;
    }

    return result;
}

void* thread_handle(void *args) {
    struct thread_args *arg = (struct thread_args *)args;
    int client_fd = arg->client_fd;
    char buffer[1000];

    client_count++; // Increment the client count
    printf("Client connected (Client %d)\n", client_count);

    while (1) {
        bzero(buffer, sizeof(buffer));

        ssize_t bytesRead = read(client_fd, buffer, sizeof(buffer));

        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                // Client closed the connection
                printf("Client %d disconnected\n", client_count);
            } else {
                perror("Error reading from client");
            }
            break;
        }

        printf("Client %d sent: %s\n", client_count, buffer);

        int n = atoi(buffer);

        long long int result = factorial(n);

        char result_str[1000];

        sprintf(result_str, "%lld", result);
        printf("Writing to client %d: %s\n", client_count, result_str);

        write(client_fd, result_str, strlen(result_str) + 1);
    }

    close(client_fd);
    free(args); // Free the thread arguments

    pthread_exit(0);
}

int main() {
    pthread_t thread_id[10];

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int bindStatus = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (bindStatus < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    int listenStatus = listen(server_fd, 10);

    if (listenStatus < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080\n");

    for (int z = 0; z < 10; z++) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd < 0) {
            perror("Accept failed");
            continue; // Continue to the next iteration
        }

        struct thread_args *args = malloc(sizeof(struct thread_args));
        args->client_fd = client_fd;
        pthread_create(&thread_id[z], NULL, thread_handle, (void *)args);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(thread_id[i], NULL);
    }

    close(server_fd);

    return 0;
}
