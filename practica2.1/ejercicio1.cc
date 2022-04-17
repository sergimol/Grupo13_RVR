#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <string.h>

int main(int argc, char** argv) {
	struct addrinfo hints;
	struct addrinfo* res;

	memset((void*)&hints, 0, sizeof(struct addrinfo));

	// Criterios de búsqueda
	hints.ai_flags = AF_UNSPEC;
	hints.ai_family = AF_UNSPEC;

	// Obtiene la información de direcciones según lo especificado en argv
	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
	if (rc != 0) {
		std::cout << gai_strerror(rc) << "\n";
		return -1;
	}

	// Escribe las direcciones 
	for (struct addrinfo* i = res; i != 0; i = i->ai_next) {
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];

		getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

		std::cout << host << " " << i->ai_family << " " << i->ai_socktype << "\n";
	}

	freeaddrinfo(res);
	return 0;
}
