//included headers
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>  
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>


typedef struct connection 
{	
	struct addrinfo p ; 
int sockfd;

}connection;


#define MAXPACKETSIZE 100
#define HEADERSIZE 8
