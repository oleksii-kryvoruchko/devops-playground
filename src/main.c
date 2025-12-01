#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#define CPU_COUNT 1
#define VM_COUNT 2 
#define VM_BYTES "500M"
#define STRESS_TIMEOUT "120s"

void execute_stress_command(const char *command) {
    system(command);
}

void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE] = {0};
    read(client_socket, buffer, BUFFER_SIZE - 1);

    if (strstr(buffer, "GET /cpu")) {
        char command[256];
        snprintf(command, sizeof(command), "nohup stress-ng --cpu %d --timeout %s > /dev/null 2>&1 &", CPU_COUNT, STRESS_TIMEOUT);
        execute_stress_command(command);
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nCPU stress-ng started");
    }
    else if (strstr(buffer, "GET /memory")) {
        char command[256];
        snprintf(command, sizeof(command), "nohup stress-ng --vm %d --vm-bytes %s --timeout %s > /dev/null 2>&1 &", VM_COUNT, VM_BYTES, STRESS_TIMEOUT);
        execute_stress_command(command);
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nMemory stress-ng started");
    }
    else if (strstr(buffer, "GET /stop")) {
        execute_stress_command("pkill -f stress-ng");
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTests stopped");
    }
    else {
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nEndpoints: /cpu /memory /stop ");
    }
    write(client_socket, response, strlen(response));
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_socket, (struct sockaddr *)&address, sizeof(address));
    listen(server_socket, 3);
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        handle_request(client_socket);
    }
    return 0;
}
