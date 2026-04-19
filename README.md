# ETN 911
## LABORATORIO 2

### 1. Objetivo.

Afianzar los conocimientos en sockets de comunicación.
Desarrollar conocimientos de gestión de archivos

### 2. Problemas a resolver.

Se requiere un servidor que proporcione a los clientes un archivo de un directorio específico

Se requiere desarrollar un software cliente especifico que permita poner nombre del archivo a descargar.

### 3. Aplicación práctica.

Una planta industrial tiene montados 5 sensores en una planta remota y una camara donde se vigila la la maquina, se requiere acceder remotamente mediante un servidor a información de la siguiente forma:

Desde un navegador web se debe visualizar:
El valor de cada sensor
Una gráfica de valores de los sensores cada 30 minutos durante 24 horas
Captura de imagen de la cámara al momento de la conexión

Desde una App movil cliente acceder a los mismos datos.

Componentes:

    • En planta.
        ◦ Raspberry Pi
        ◦ Sensores según el proceso (temperatura, humedad, presion, nivel, etc.)
        ◦ Cámara conectada al Raspberry Pi
        ◦ Acceso a Internet
        ◦ IP fijo

    • En remoto:
        ◦ Mobil Android



#### Ingeniería:

Evaluar costo de solución basada en Raspberry Pi versus solución industrial
Evaluar disponibilidad de la solución basada en Raspberry Pi versus solución industrial

### 2. Desarrollo.

#### Requisitos

    • Ubuntu on premise, dual boot, virtual o WSL
    • Compilador GNU C (gcc)
    • Editor nano o vi

#### Código fuente servidor.
``` c
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
```

#### Código fuente del cliente
``` c
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
````

### 3. Desafíos Propuestos
#### Desafío 1: Implementación de barra de progreso en el cliente

Diseñar e implementar un mecanismo en el cliente que permita mostrar el avance de la descarga del archivo solicitado al servidor. El sistema debe reflejar el porcentaje de datos recibidos en función del total del archivo, actualizándose en tiempo real durante la transferencia. Se debe considerar cómo obtener o estimar el tamaño total del archivo para calcular el progreso.

#### Desafío 2: Soporte para múltiples solicitudes en una sola conexión

Extender el sistema para que un cliente pueda solicitar varios archivos sin necesidad de cerrar y reabrir la conexión con el servidor. El servidor deberá permanecer atendiendo múltiples peticiones consecutivas del mismo cliente hasta que este indique explícitamente la finalización de la sesión. Se debe diseñar un mecanismo de control de flujo entre cliente y servidor.

#### Desafío 3: Hacker el sistema

Usar el cliente para obtener el archivo /etc/passwd sin modificar los codigos fuentes originales.

#### Desafío 4: Implementación de validación de rutas y seguridad básica

Desarrollar un sistema de validación en el servidor que impida el acceso a archivos fuera del directorio autorizado. El servidor debe analizar las solicitudes del cliente y rechazar aquellas que intenten acceder a rutas inválidas o potencialmente peligrosas. Se debe garantizar que únicamente se puedan descargar archivos dentro de un directorio específico del servidor.
