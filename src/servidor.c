#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void manejar_cliente(int client_sock) {
    char filename[256];
    char buffer[BUFFER_SIZE];
    
    memset(filename, 0, sizeof(filename));
    read(client_sock, filename, sizeof(filename));

    printf("Cliente solicita archivo: %s\n", filename);

    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        strcpy(buffer, "ERROR: Archivo no encontrado\n");
        write(client_sock, buffer, strlen(buffer));
    } else {
        int bytes;
        while ((bytes = read(fd, buffer, BUFFER_SIZE)) > 0) {
            write(client_sock, buffer, bytes);
        }
        close(fd);
    }

    close(client_sock);
    exit(0);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    printf("Servidor escuchando en puerto %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);

        if (fork() == 0) {
            close(server_sock);
            manejar_cliente(client_sock);
        }

        close(client_sock);
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }

    return 0;
}
