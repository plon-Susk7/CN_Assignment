#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <poll.h>

#define MAX_CONNECTIONS 4000

long long int computeFactorial(int n);

int main() {
    int mainSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (mainSocket < 0) {
        perror("Main Socket Could Not be Created");
        exit(1);
    }

    struct pollfd pollFDs[MAX_CONNECTIONS + 1];  // +1 for the mainSocket
    int maxFDs = 1;

    FILE *fp = fopen("PollResults.txt", "w+");

    if (fp == NULL) {
        perror("File Could Not be Opened");
        exit(1);
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int bindResponse = bind(mainSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    if (bindResponse < 0) {
        perror("Bind Failed");
        exit(1);
    }

    int listenResponse = listen(mainSocket, MAX_CONNECTIONS);

    if (listenResponse < 0) {
        perror("Listen Failed");
        exit(1);
    }

    printf("Server is Listening...\n");

    pollFDs[0].fd = mainSocket;
    pollFDs[0].events = POLLIN;

    while (1) {
        int pollResponse = poll(pollFDs, maxFDs, -1);

        if (pollResponse < 0) {
            perror("Poll Failed");
            exit(1);
        }

        for (int i = 0; i < maxFDs; i++) {
            if (pollFDs[i].revents & POLLIN) {
                if (pollFDs[i].fd != mainSocket) {
                    // Handle client data
                    char receiving_buffer[1000];
                    bzero(receiving_buffer, 1000);
                    int readResponse = read(pollFDs[i].fd, receiving_buffer, 1000);

                    if (readResponse < 0) {
                        perror("Read Failed");
                        exit(1);
                    }
                    if (readResponse == 0) {
                        close(pollFDs[i].fd);
                        pollFDs[i].fd = -1;
                    } else {
                        int value_received = atoi(receiving_buffer);
                        long long int factorial = computeFactorial(value_received);
                        char sending_buffer[1000];

                        printf("Client %d requested factorial of %d, which is %lld\n", i, value_received, factorial);

                        snprintf(sending_buffer, 1000, "%lld", factorial);
                        send(pollFDs[i].fd, sending_buffer, 1000, 0);
                    }
                } else {
                    // Accept new connections and add to pollFDs
                    struct sockaddr_in clientData;
                    socklen_t clientDataLength = sizeof(clientData);
                    int newConnection = accept(mainSocket, (struct sockaddr *)&clientData, &clientDataLength);

                    if (newConnection < 0) {
                        perror("Accept Failed");
                        exit(1);
                    }

                    for (int j = 1; j < MAX_CONNECTIONS + 1; j++) {
                        if (pollFDs[j].fd == -1) {
                            pollFDs[j].fd = newConnection;
                            pollFDs[j].events = POLLIN;
                            if (j >= maxFDs) {
                                maxFDs = j + 1;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    fclose(fp);
    close(mainSocket);
    return 0;
}

long long int computeFactorial(int n) {
    long long int fact = 1;
    for (int i = 1; i <= n; i++)
        fact *= i;
    return fact;
}
