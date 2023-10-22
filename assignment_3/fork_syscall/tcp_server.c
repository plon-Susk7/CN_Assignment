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

    struct sockaddr_in client_addr;

    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

    if(client_fd < 0){
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected\n");

    char buffer[1024];
    for(int i=0;i<20;i++){
        bzero(buffer, sizeof(buffer));
        int read_status = read(client_fd, buffer, sizeof(buffer));

        if(read_status < 0){
            perror("Read failed");
            exit(EXIT_FAILURE);
        }

        printf("Client sent: %s\n", buffer);

        int n = atoi(buffer);

        long long int result = factorial(n);

        char result_str[1024];

        sprintf(result_str, "%lld", result);

        int write_status = write(client_fd, result_str, sizeof(result_str));

        if(write_status < 0){
            perror("Write failed");
            exit(EXIT_FAILURE);
        }

        printf("Result sent to client\n");
    }


   

    close(client_fd);

    close(server_fd);









    return 0;
}