#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <time.h>

void processMsg(struct sockaddr* addr, socklen_t addrlen, char command, int sd, bool& exit, char* msg) {
    time_t t;
    char timeInfo[80];
    size_t bytes;

    switch (command)
    {
        // Devuelve la hora
    case 't':
        time(&t);
        bytes = strftime(timeInfo, sizeof(timeInfo), "%T", localtime(&t));

        sendto(sd, timeInfo, bytes, 0, addr, addrlen);
        break;
        // Devuelve la fecha
    case 'd':
        time(&t);
        bytes = strftime(timeInfo, sizeof(timeInfo), "%F", localtime(&t));

        sendto(sd, timeInfo, bytes, 0, addr, addrlen);
        break;
        // Cerrar la conexión
    case 'q':
        exit = true;
        strcpy(msg, "Saliendo...");
        sendto(sd, "", 0, 0, addr, addrlen);
        break;
    default:
        strcpy(msg, "Comando no soportado.");
        sendto(sd, "", 0, 1, addr, addrlen);
        break;
    }
}

int main(int argc, char** argv) {
    struct addrinfo hints;
    struct addrinfo* res;

    memset((void*)&hints, 0, sizeof(struct addrinfo));
    // Parámetros de búsqueda
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_UNSPEC;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (rc != 0) {
        std::cout << gai_strerror(rc) << "\n";
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if (sd == -1) {
        std::cout << gai_strerror(sd) << "\n";
        return -1;
    }

    // Asigna la dirección al socket
    bind(sd, res->ai_addr, res->ai_addrlen);

    bool exit = false;
    while (!exit) {
        // Nombres de host y de servidor
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        // Dirección del socket
        struct sockaddr src_addr;
        socklen_t addrlen = sizeof(struct sockaddr);
        char buffer[80];
        char msg[80] = "";

        // Recibe el mensaje a través del socket
        ssize_t nBytes = recvfrom(sd, (void*)buffer, sizeof(buffer), 0, &src_addr, &addrlen);
        buffer[nBytes] = '\0';

        getnameinfo(&src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        processMsg(&src_addr, addrlen, buffer[0], sd, exit, msg);

        std::cout << nBytes << " bytes de " << host << ":" << serv << "\n";
        std::cout << msg << "\n";
    }

    freeaddrinfo(res);
    return 0;
}