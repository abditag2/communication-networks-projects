
#ifndef ROUTER_H
#define ROUTER_H


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




#include "linked_list.c"

#define MAXBUFLEN 1024
#define MAX_COMMAND_SIZE 500 //maximum string length for a received command


	/* Represents a packet received from the server.
	 * Each packet is pushed onto stack by the reader thread.
	 * The writer thread Pops each packet and writes it
	 * to the output file.
	 */

	int numRoutes  = 0 ;


	void *get_in_addr(struct sockaddr *) ;

	/* Retrieves a command from the server.
	 *
	 * sockfd: file descriptor for the socket we will retrieve the message from
	 * msg_arr: destination array for the command string
	 * sentinel: a string that indicates the end of the message
	 */
	int get_command(int , char * , char * );
	int udp_init(node* const, node * ) ;
	int single_udp_setup(node* ) ;
	int max(int  , int ) ;
	void setnonblocking(int);
	void broadcastRT(nodeRT* , node* , node * ) ;
	int whatIsNextHop(int , nodeRT* , int ) ;
	int isInThePath(char ** path, char*) ;


#endif

