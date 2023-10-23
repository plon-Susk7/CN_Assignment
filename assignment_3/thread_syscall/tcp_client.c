#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>


void *getFactorial(void *arg){

    char* address = "127.0.0.1";

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr(address);

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(client_fd < 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int connect_status = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if(connect_status < 0){
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    int i =1;

    while(i<=20){
        printf("Sending %d\n", i);
        
        char message[1000];
        snprintf(message, 1000,"%d", i);
        write(client_fd, message, sizeof(message));

        char buffer[1000];
        read(client_fd, buffer, sizeof(buffer));
        printf("Received %s\n", buffer);

        i++;
    }

    close(client_fd);

}

int main(){

    pthread_t thread_id[10];

    for(int i=0;i<10;i++){
        pthread_create(&thread_id[i], NULL, getFactorial, NULL);
    }

    for(int i=0;i<10;i++){
        pthread_join(thread_id[i], NULL);
    }
    
    return 0;
}