#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>


struct thread_args{
    int client_fd;
};

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

void* thread_handle(void *args){

    struct thread_args *arg = (struct thread_args *)args;

    int client_fd = arg->client_fd;

    char buffer[1000];

    for(int i=1; i<=20; i++){
        read(client_fd, buffer, sizeof(buffer));
        printf("Received %s\n", buffer);

        long long int result = factorial(atoi(buffer));

        char message[1000];

        snprintf(message, 1000, "%lld", result);
        printf("Sending %s\n", message);
        write(client_fd, message, sizeof(message));
    }

    close(client_fd);

    return NULL;
}

int main(){
    pthread_t thread_id[10];

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(server_fd < 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int bind_status = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if(bind_status < 0){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    int listen_status = listen(server_fd, 10); // the second argument (0) represents maximum number of queued connections

    if(listen_status < 0){
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080\n");



    for(int z=0;z<10;z++){

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if(client_fd < 0){
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Client connected\n");

        struct thread_args *args = malloc(sizeof(struct thread_args));
        args->client_fd = client_fd;

        pthread_create(&thread_id[z], NULL, thread_handle, (void *)args);


    }

    for(int i=0;i<10;i++){
        pthread_join(thread_id[i], NULL);
    }

    

    
    
    return 0;
}