#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/select.h>
#include <netinet/in.h>

#define SERVER_PORT 8989
#define BUFFER_SIZE 4096
#define SOCKET_ERROR (-1)
#define SERVER_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

//prototypes
void * handle_connection(int);
int check(int exp, const char *msg);
int accept_new_connection(int server_socket);
int setup_server(short port, int backlog);

int main(int argc, char **argv)
{
    int server_socket = setup_server(SERVER_PORT, SERVER_BACKLOG);

    int max_socket = 0;

    fd_set current_sockets, ready_sockets;

    //inicializar set atual
    FD_ZERO(&current_sockets);
    FD_SET(server_socket, &current_sockets);
    max_socket = server_socket;

    printf("FD_SETSIZE = %d\n", FD_SETSIZE);

    while (true)
    {   
        ready_sockets = current_sockets;

        if (select(max_socket, &ready_sockets, NULL, NULL, NULL) < 0)
        {
            perror("Failed to select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < max_socket; i++)
        {
            if (FD_ISSET(i, &ready_sockets))
            {
                if (i == server_socket)
                {   
                    //nova conexão
                    int client_socket = accept_new_connection(server_socket);
                    FD_SET(client_socket, &current_sockets);

                    if (client_socket > max_socket)
                    {
                        max_socket = client_socket;
                    }
                } 
                else
                {
                    handle_connection(i);
                    FD_CLR(i, &current_sockets);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

int setup_server(short port, int backlog)
{
    int server_socket, client_socket, addr_size;
    SA_IN server_addr;

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)), 
        "Failed to create socket");
    
    //inicializar struct do endereço
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    check(bind(server_socket, (SA*) &server_addr, sizeof(server_addr)),
        "Failed to bind");

    check(listen(server_socket, backlog),
        "Failed to listen");

    return server_socket;
}

int accept_new_connection(int server_socket)
{
    int addr_size = sizeof(SA_IN);
    int client_socket;
    SA_IN client_addr;

    check(client_socket = accept(server_socket, (SA*) &client_addr, (socklen_t*) &addr_size), 
        "Failed to accept");
    
    return client_socket;
}

int check(int exp, const char *msg)
{
    if (exp == SOCKET_ERROR)
    {
        perror(msg);
        exit(1);
    }

    return exp;
}

void * handle_connection(int client_socket)
{
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int msgsize = 0;
    char actualpath[PATH_MAX+1];

    //ler a mensagem do client - nome do arquivo para ler
    while((bytes_read = read(client_socket, buffer+msgsize, sizeof(buffer)-msgsize-1)))
    {
        msgsize += bytes_read;

        if (msgsize > BUFFER_SIZE-1 || buffer[msgsize-1] == '\n') break;
    }

    check(bytes_read, "recv error");
    buffer[msgsize-1] = 0; //null termina a mensagem e remove o \n

    printf("REQUEST: %s\n", buffer);
    fflush(stdout);

    //checagem de validação
    if (realpath(buffer, actualpath) == NULL)
    {
        printf("ERROR(bad path): %s\n", buffer);
        close(client_socket);
        return NULL;
    }

    //ler arquivo e enviar seu conteúdo para o client
    FILE *fp = fopen(actualpath, "r");
    if (fp == NULL)
    {
        printf("ERROR(open): %s\n", buffer);
        close(client_socket);
        return NULL;
    }

    //ler conteúdo do arquvio e enviar para o client
    //exemplo inseguro, um programa real limitaria o client por tipos de arquivo
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0)
    {
        printf("sending %zu bytes\n", bytes_read);
        write(client_socket, buffer, bytes_read);
    }

    close(client_socket);
    fclose(fp);
    printf("closing connection\n");

    return NULL;
}