#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
/*
struct sockaddr{
	
}

struct addrinfo{
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	socklen_t ai_addrlen;
	struct sockaddr *ai_addr;
	char *ai_cannoname;
	struct addrinfo *ai_next;
}*/

int main(int argc, char **argv){
	addrinfo* hints;
	hints.ai.flags = AI_PASSIVE;
	hints.ai.family = AF_INET;
	hints.ai.family = SOCK_DGRAM;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
	int sd = socket(result->ai_family, result->ai_socktype, 0);

	bind(sd, (struct sockaddr *) result->ai_addr, result->(ai_addrlen);

	getnameinfo((struct sockaddr * ) 
	return 0;
}
