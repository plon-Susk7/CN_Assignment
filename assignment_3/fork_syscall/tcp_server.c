#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <fcntl.h> 
#include <unistd.h>


long long int factorial(int n){
    if(n==0){
        return 1;
    }

    if(n>20) n=20;

    long long int result = 1;

    for(int i=1; i<=n; i++){
        result *= i;
    }

    return result;
    
}


int main(){
    struct sockaddr_in server_addr;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(server_fd < 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bind_status = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if(bind_status < 0){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    int listen_status = listen(server_fd, 0); // the second argument (0) represents maximum number of queued connections

    if(listen_status < 0){
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080\n");
    printf("Waiting for client...\n");


    int client_fd;

    for(int z=0;z<10;z++){
        struct sockaddr_in client_addr;

        socklen_t client_addr_len = sizeof(client_addr);

        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if(client_fd < 0){
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Client connected\n");

        if(fork() == 0){
        
            close(server_fd);
            char buffer[1000];
            for(int i=0;i<20;i++){
                bzero(buffer, sizeof(buffer));
                read(client_fd, buffer, sizeof(buffer));

            

                printf("Client sent: %s\n", buffer);

                int n = atoi(buffer);

                long long int result = factorial(n);

                char result_str[1000];

                sprintf(result_str, "%lld", result);
                printf("Writing to client: %s\n", result_str);
                write(client_fd, result_str, sizeof(result_str));
            }
            exit(0);
        }


    }

    close(client_fd);

    close(server_fd);


    return 0;
}