#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char filename[256];
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Ingrese el nombre del archivo: ");
    scanf("%s", filename);

    write(sock, filename, sizeof(filename));

    FILE *fp = fopen("archivo_recibido", "wb");

    int bytes;
    while ((bytes = read(sock, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, bytes, fp);
    }

    fclose(fp);
    close(sock);

    printf("Archivo recibido y guardado como 'archivo_recibido'\n");

    return 0;
}
