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


#define MAXPACKETSIZE 100
#define HEADERSIZE 8 
#define SENDWINDOWSIZE 5
#define TIMEOUTINIT 1500000
#define SENDBUFSIZE 20 
#define ALPHA 750

typedef struct connection 
{
	struct sockaddr_storage their_addr;
	socklen_t addr_len;
	int sockfd;

}connection;

void *get_in_addr(struct sockaddr *sa);

int init_udp(char * port, connection * listener);

