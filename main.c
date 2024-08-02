#include <stdio.h>
#include <stdlib.h>
#include "proxy/proxy.h"
#include "sandbox/sandbox.h"

int sandbox_callback(const char *request) {
    return 0;
}

int main() {
    int port = 8888; // Proxy server port
    char *server_address = "127.0.0.1"; // server addr
    int server_port = 8080; // server port

    start_proxy_server(port, server_address, server_port);

    return 0;
}
