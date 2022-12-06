#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE 1024

int flag;
int client_socket;

void send_msg_handler()
{
    char send_message[BUF_SIZE] = {};
    while (1)
    {
        while (fgets(send_message, BUF_SIZE, stdin) != NULL)
        {
            if (strlen(send_message) == 0)
                continue;
            else break;
        }
        send(client_socket, send_message, BUF_SIZE, 0);
    }
    
}

void recv_msg_handler()
{
    char recv_message[BUF_SIZE] = {};
    while (1)
    {
        int bytes = recv(client_socket, recv_message, BUF_SIZE, 0);
        if (bytes > 0)
            printf("%s\n", recv_message);
        else if (bytes < 0)
        {
            printf("Error get message\n");
            exit(1);
        }
    }
}

int main()
{
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        printf("Error create socket\n");
        exit(1);
    }

    struct sockaddr_in server_address;

    server_address.sin_family = PF_INET;
    server_address.sin_port = htons(8000);
    server_address.sin_addr.s_addr = inet_addr("0.0.0.0");

    int err = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (err == -1)
    {
        printf("Error connection to server\n");
        exit(1);
    }

    printf("Connect to server: %s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0) {
        printf ("Error create pthread\n");
        exit(1);
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0) {
        printf ("Error create pthread\n");
        exit(1);
    }

    while (1) {
        if(flag) {
            printf("\nEXIT\n");
            break;
        }
    }

    close(client_socket);
    return 0;
}