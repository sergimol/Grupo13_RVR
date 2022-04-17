#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

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
    freeaddrinfo(res);

    listen(sd, 16);

    bool exit = false;
    while (!exit){
        // Nombres de host y de servicio
        char host[NI_MAXHOST];
		char serv[NI_MAXSERV];

        // Direcci�n del socket
        struct sockaddr src_addr;
        socklen_t addrlen = sizeof(struct sockaddr);
        char msg[80] = "";

        int sd_client = accept(sd, &src_addr, &addrlen);
        getnameinfo(&src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "Conexi�n desde " << host << " " << serv << "\n";

        while(!exit){
            char buffer[80];
            
            ssize_t nBytes = recv(sd_client, (void*)buffer, sizeof(buffer), 0);
            buffer[nBytes] = '\0';

            send(sd_client, buffer, nBytes, 0);

            if (nBytes == 0 || !strcmp(buffer, "Q") || !strcmp(buffer, "q"))
                exit = true;
        }
    }
    std::cout << "Conexi�n terminada\n";
    return 0;
}