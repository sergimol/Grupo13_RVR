#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>

class MessageThread{
    public:
        MessageThread(int s):sd(s){};
        virtual ~MessageThread(){};
        void processMsg(){
            while(!fin){
                char buffer[256];
                ssize_t size = recv(sd, &buffer, 255, 0);

                if(size == 0){
                    fin = true;
                    std::cout << "Saliendo...\n";
                }
                else{
                    send(sd, &buffer, size, 0);
                    std::cout << "Hilo: " << pthread_self() << "\n";
                }
            }
        }
    private:
        int sd;
        bool fin;
};

extern "C" void *_process_Msg(void* _mt){
    MessageThread* mt = static_cast<MessageThread*>(_mt);
    mt->processMsg();
    delete mt;
    return 0;
}

int main(int argc, char **argv){
	struct addrinfo hints;
    struct addrinfo* res;

	memset((void*)&hints, '\0', sizeof(struct addrinfo));
    // Parámetros de búsqueda
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;

    // Nombres de host y de servidor
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

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

    struct sockaddr client;
    socklen_t client_len = sizeof(client);
    char c = 'p';

    while (true){
        int nThread = accept(sd, (struct sockaddr*) &client, &client_len);

        pthread_t tid;
        pthread_attr_t attr;

        MessageThread* mt = new MessageThread(nThread);

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&tid, &attr, _process_Msg, static_cast<void*>(mt));

        while (c != 'q' && c != 'Q')
        {
            std::cin >> c;
        }
        
    }
    
    return 0;
}