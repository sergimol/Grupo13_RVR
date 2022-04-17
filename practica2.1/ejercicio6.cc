#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define THREADS 5

class MessageThread{
    public:
        MessageThread(int s):sd(s){};
        virtual ~MessageThread(){};
        void processMsg(){
            while(true){
                char host[NI_MAXHOST];
                char serv[NI_MAXSERV];
                char msg[256];
                struct sockaddr soc;
                socklen_t addrlen = sizeof (soc);

                ssize_t size = recvfrom(sd, msg, 255, 0, &soc, &addrlen);
                getnameinfo(&soc, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);
                std::cout << size << " bytes de " << host << " " << serv << " Hilo: " << pthread_self() << "\n";

                char buffer[256];
                memset((void*) buffer, '\0', 256);
                time_t t;
                struct tm *info;
                
                switch (msg[0])
                {
                // Devuelve la hora
                case 't':
                    time(&t);
                    info = localtime(&t);
                    bytes = strftime(buffer, 80, "%r", info);

                    buffer[bytes] = '\0';
                    sendto(sd, buffer, 256, 0, &soc, addrlen);
                    break;
                // Devuelve la fecha
                case 'd':
                    time(&t);
                    info = localtime(&t);
                    bytes = strftime(buffer, 80, "%F", info);

                    buffer[bytes] = '\0';
                    sendto(sd, buffer, 256, 0, &soc, addrlen);
                    break;
                // Cerrar la conexión
                case 'q':
                    std::cout << "Saliendo...\n";
                    break;
                default:
                    std::cout << "Comando no soportado.\n";
                    break;
                }
            }
        }
    private:
        int sd;
        size_t bytes;
};

extern "C" void *_process_Msg (void * _mt){
    MessageThread * mt = static_cast<MessageThread*>(_mt);
    mt->processMsg();
    delete mt;
    return 0;
}

int main(int argc, char **argv){
	struct addrinfo hints;
    struct addrinfo* res;

	memset((void*)&hints, 0, sizeof(struct addrinfo));
    // Parámetros de búsqueda
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_INET;

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
    listen(sd, 15);
    freeaddrinfo(res);

    for(int i = 0; i <= THREADS; ++i){
        pthread_t tid;
        pthread_attr_t attr;

        MessageThread* mt = new MessageThread(sd);

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        pthread_create(&tid, &attr, _process_Msg, static_cast<void*>(mt));
    }

    char c = 'p';
    while(c != 'q'){
        std::cin >> c;
    }

	freeaddrinfo(res);
	close(sd);

    return 0;
}