#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "sandbox.h"


// Func to make request in sandbox
int execute_in_sandbox(const char *request) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int result = 1; // Request is harmful by default

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error \n");
        return result;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);

    // IPv4 и IPv6 from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported \n");
        return result;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed \n");
        return result;
    }

    char json_request[256];
    snprintf(json_request, sizeof(json_request), "{\"request\":\"%s\"}", request);
    send(sock, json_request, strlen(json_request), 0);
    printf("Request sent: %s\n", json_request);

    int valread = read(sock, buffer, 1024);
    printf("Response received: %s\n", buffer);

    // Check response from sandbox
    if (strstr(buffer, "\"status\":\"safe\"") != NULL) {
        result = 0; // Request is safe
    }

    close(sock);
    return result;
}