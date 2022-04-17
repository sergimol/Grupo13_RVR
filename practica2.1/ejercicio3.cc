#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char **argv){
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
    if(sd == -1){
        std::cout << gai_strerror(sd) << "\n";
        return -1;
    }

    // Asigna la dirección al socket
    bind(sd, res->ai_addr, res->ai_addrlen);

    // Para guardar la dirección del socket
    struct sockaddr src_addr;
    socklen_t addrlen = sizeof(struct sockaddr);
    char buffer[80];

    src_addr = *res->ai_addr;
    strcpy(buffer, argv[3]);

    // Solicitud al servidor
    sendto(sd, buffer, sizeof(buffer), 0, &src_addr, addrlen);

    // Recepción del mensaje
    ssize_t nBytes = recvfrom(sd, (void*)buffer, sizeof(buffer), 0, &src_addr, &addrlen);
    buffer[nBytes] = '\0';

    std::cout << buffer << "\n";
}