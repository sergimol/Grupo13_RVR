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
    // Par�metros de b�squeda
	hints.ai_socktype = SOCK_STREAM;
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

    // Asigna la direcci�n al socket
    bind(sd, res->ai_addr, res->ai_addrlen);

    // Conexi�n al servidor
    connect(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    while (true)
    {
        socklen_t addrlen = sizeof(struct sockaddr);
        char buffer[80];
        
        std::cin >> buffer;
        // Env�o de mensaje al servidor
        send(sd, buffer, sizeof(buffer), 0);

        // Condici�n de desconexi�n
        if(!strcmp(buffer, "Q") || !strcmp(buffer, "q"))
            break;
        
        // Recepci�n de la respuesta del servidor
        ssize_t nBytes = recv(sd, (void*)buffer, sizeof(buffer), 0);
        buffer[nBytes] = '\0';
        
        std::cout << buffer << "\n";
    }
}