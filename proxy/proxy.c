#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "proxy.h"
#include "security.h"
#include "sandbox.h"

#define BUFFER_SIZE 4096

void forward_request(int client_socket, int server_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Read request from client
    while ((bytes_read = read(client_socket, buffer, BUFFER_SIZE)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Received request: %s\n", buffer);
        // Handle some obvious threats; No need to execute in sandbox
        if (check_security(buffer)) {
            printf("threat detected!\n");
            return;
        }
        if (execute_in_sandbox(buffer)) {
            printf("Threat confirmed. Blocking request.\n");
            close(client_socket);
            close(server_socket);
            return;
        } else {
            printf("No threat detected in sandbox. Forwarding request.\n");
        }
    }

        // Forward request to server
        write(server_socket, buffer, bytes_read);
}


void forward_response(int server_socket, int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Read response from server
    while ((bytes_read = read(server_socket, buffer, BUFFER_SIZE)) > 0) {
        // Forward response to client
        write(client_socket, buffer, bytes_read);
    }
}

void start_proxy_server(int port, const char *server_address, int server_port) {
    int proxy_fd, client_socket, server_socket;
    struct sockaddr_in proxy_addr, client_addr, server_addr;
    int addrlen = sizeof(proxy_addr);

    // Proxy server creation
    if ((proxy_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_addr.s_addr = INADDR_ANY;
    proxy_addr.sin_port = htons(port);

    if (bind(proxy_fd, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) < 0) {
        perror("Bind failed");
        close(proxy_fd);
        exit(EXIT_FAILURE);
    }

     if (listen(proxy_fd, 3) < 0) {
        perror("Listen failed");
        close(proxy_fd);
        exit(EXIT_FAILURE);
    }

    while ((client_socket = accept(proxy_fd, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen)) >= 0) {
        // Socket creation to connect to server
        if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Server socket failed");
            close(client_socket);
            continue;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port);

        // Main server address
        if (inet_pton(AF_INET, server_address, &server_addr.sin_addr) <= 0) {
            perror("Invalid address/ Address not supported");
            close(client_socket);
            close(server_socket);
            continue;
        }

        // Connect to main server
        if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection to server failed");
            close(client_socket);
            close(server_socket);
            continue;
        }

        // Req and resp redirection
        forward_request(client_socket, server_socket);
        forward_response(server_socket, client_socket);

        close(client_socket);
        close(server_socket);
    }

    if (client_socket < 0) {
        perror("Accept failed");
        close(proxy_fd);
        exit(EXIT_FAILURE);
    }

    close(proxy_fd);
}
