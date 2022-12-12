#include <stdio.h>
#include <stdlib.h>
// #include <sys/types.h>
// #include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server.h"

#define PORT 8000
#define BUF_SIZE 1024

ClientList *client_list;

void send_to_all_clients(char buffer[])
{
    ClientList *tmp = client_list->next;
    while (tmp != NULL)
    {
        if(client_list->data != tmp->data)
        {
            printf("Send to sockfd %d: \"%s\" \n", tmp->data, buffer);
            send(tmp->data, buffer, BUF_SIZE, 0);
        }
        tmp = tmp->next;
    }
    
}

void client_handler(int *client)
{
    int recv_buffer[BUF_SIZE] = {};
    int send_buffer[BUF_SIZE] = {};
    ClientList *c = (ClientList *)client;

    while(1)
    {
        int bytes = recv(c->data, recv_buffer, BUF_SIZE, 0);
        if (bytes > 0)
        {
            sprintf(send_buffer, "%s", recv_buffer);
        } else {
            continue;
        }
        send_to_all_clients(send_buffer);
    }
}

int main()
{
    printf("Server starting...\n");
    
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("Error create socket\n");
        exit(1);
    }

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    //int s_addrlen = sizeof(server_socket);

    server_address.sin_family = PF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    listen(server_socket, 4);

    printf("Server start: %s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
    printf("Listening...\n");

    client_list = newNode(server_socket);
    ClientList *this = client_list;

    while (1)
    {
        int client_socket;

        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0)
        {
            printf("Error accept call failed\n");
            continue;
        }

        getpeername(client_socket, (struct sockaddr*) &client_address, sizeof(client_address));
        printf("Client %s:%d connect\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        ClientList *new_client = newNode(client_socket);
        this->next = new_client;
        this = new_client;

        pthread_t id;
        if (pthread_create(&id, NULL, (void *)client_handler, (void *)new_client) != 0)
        {
            printf("Error create pthread\n");
            exit(1);
        }
    }
    
    close(server_socket);
    return 0;
}
