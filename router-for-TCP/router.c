#include "router.h"

#define MAXDATASIZE 1024
#define _POSIX_X_SOURCE 199309
#define MAXNODENUMBER 50  // number of nodes
#define MAXRTCONTENTPACKETSIZE 1024


int main(int argc, char *argv[]){

	nodeRT routingTable[MAXNODENUMBER] ; 
	int c,m ; 
	for ( c = 0 ; c < MAXNODENUMBER ; c++ ) 
		for(m = 0 ; m < PATHLENGTH ; m++)
			strcpy(routingTable[c].path[m], "E") ; 
	
	node my_node ; 
		
	if (argc != 4) {
		fprintf(stderr,"not enough arguments, usage: ./router hostname managerport listenon\n");
	    exit(1);
	}

	char hostname[50], managerport[50]; //holds the file path of output file (string)	


	strcpy(hostname, argv[1] ) ;  
	strcpy(managerport, argv[2] ) ; 
	strcpy(my_node.udpport, argv[3] ) ; 


//--The following code is from Beej's guide--//


	int sockfd; //initialize the structures for creating a new socket
	volatile numbytes;
	unsigned char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];



	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, managerport, &hints, &servinfo)) != 0) {

		fprintf(stderr, "getaddrinfo: here %s\n", gai_strerror(rv));
		return 1;
	}


	
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}


	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
			
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure	
//-- end of refrenced code


// begin handshake with manager, send HELO
	if( send(sockfd, "HELO\n", 5, 0) != 5 ) 
		fprintf(stderr, "client: failed to send HELO\n");


	char addr[10] ; 
	char* recv_msg ; 

// manager replies with address assinged to this client
	get_command(sockfd, buf, "\n");
	strtok(buf, " " ) ;
	strcpy(my_node.addr , strtok(NULL, " \n" )) ; 

	printf( "manager replied with address %s\n", my_node.addr);

	char msg[80] ; 
	sprintf(msg, "HOST %s %s\n", hostname, my_node.udpport); 

	printf("%s", msg ) ;


// send manager client info

	if( send(sockfd, msg , strlen(msg) , 0) != strlen(msg)  ) 
		fprintf(stderr, "client: failed to send HELO\n");

// manager return OK
	get_command(sockfd, buf, "OK\n");


// client requests neighbors
	if( send(sockfd, "NEIGH?\n" , 7, 0) != 7 ) 
		fprintf(stderr, "client: failed to send NEIGH?\n");


	strcpy(buf,"") ; 


	//initliaze the linked_list

        node *start,*temp;
        start = (node *)malloc(sizeof(node)); 
        temp = start;
        temp -> next = NULL;







        /* Here in this code, we take the first node as a dummy node.
           The first node does not contain data, but it used because to avoid handling special cases
           in insert and delete functions.

         */

	char temp_msg_arr[MAX_COMMAND_SIZE];

	// manager returns list of neighbors
	get_command(sockfd, temp_msg_arr, "DONE\n");

	char* temp_msg ; 
	temp_msg = strtok(temp_msg_arr," \0") ; // point to the first word
	printf("%s\n", temp_msg_arr ) ; // for debugging


	numRoutes = 0 ; 

	while(1)
	{

	// See if we are finished
	if( strcmp(temp_msg,"DONE\n") == 0 ) 
	{
		printf("DONE\n") ; 
		break ;
	}

			if (strcmp(temp_msg, "NEIGH") == 0) 
			{	
				node* new_node ;
				new_node = get_new_node() ; 


				// Fill in address
				temp_msg = strtok(NULL," \0") ; 
				strcpy(new_node->addr , temp_msg) ;
				strcpy(routingTable[numRoutes].addr , temp_msg) ;
				strcpy(routingTable[numRoutes].nextHop , temp_msg) ;
				

				// FIll in hostname
				temp_msg = strtok(NULL," \0") ;
				strcpy(new_node->hostname, temp_msg );

				// Fill in UDP port
				temp_msg = strtok(NULL," \0") ;
				strcpy(new_node->udpport, temp_msg );

				// Fill in cost
				temp_msg = strtok(NULL," \n");
				//strcpy(temp_msg_arr, temp_msg) ;
				new_node->cost = atoi(temp_msg);
				routingTable[numRoutes].cost = atoi(temp_msg);

				insert(start, new_node) ;

				

				//Trim the rest of the line
				temp_msg = strtok(NULL," \0");
				numRoutes ++ ; 
			}
	}
	

	//set the routing table entry for the node itself

	routingTable[numRoutes].cost = 0 ;
	strcpy(	routingTable[numRoutes].nextHop, my_node.addr) ;
	strcpy(	routingTable[numRoutes].addr, my_node.addr) ;  
	numRoutes ++ ; 
/*
	int j ; 
	printf("start\n");
	for ( j = 0 ; j < numRoutes ; j ++ ) 
		printf("Routing Table: ad:%s cost:%d NH:%s\n" , routingTable[j].addr,routingTable[j].cost, routingTable[j].nextHop);
*/

	//UDP connections set up
	printf ("before %d\n" , my_node.fd ) ; 
	udp_init(start, &my_node) ; 
	printf ("after %d\n" , my_node.fd ) ; 
	// setup my own FD for UDP connection

	




	if( send(sockfd, "READY\n" , 6, 0) != 6 ) 
		fprintf(stderr, "client: failed to send NEIGH?\n");

	get_command(sockfd, buf, "OK\n");
	recv_msg = strtok(buf, " \n" ) ;
	printf("%s\n", recv_msg ) ; 




// Begin Logging
	int length = strlen("LOG ON\n");
	if( send(sockfd, "LOG ON\n", length, 0) != length )
		fprintf(stderr, "client: failed to send LOG ON\n");
	get_command(sockfd, buf, "LOG ON\n");
	printf("received : %s\n", buf) ;
	setnonblocking(my_node.fd) ; 


//check for the messages from the manager

	node* node_to_be_changed;
	unsigned char msg_arr[MAXBUFLEN] ; 
	unsigned char data[MAXBUFLEN] ;
	unsigned char buf_cpy[MAXBUFLEN] ;
	unsigned char buf_tmp[MAXBUFLEN] ;

	//printf("here\n") ; 



	int counter = 0 ;
	int waitMore = 0 ; 
	int counterRoute = 0 ;

	while (1) 
	{
		counter ++ ; 
				//printf("counterRoute : %d", counterRoute);
		// For UDP receives
		if (counter == 20000){
			broadcastRT(routingTable, start, &my_node ) ;
			counter = 0 ;
		} 


		if (waitMore == 1)
		{
//			printf("waitmore =1") ;
			//printf("buf_cpy: %s\n", buf_cpy) ; 
			sprintf(data, "%s", &buf_cpy[3]);
			unsigned char udp_msg_type ; 
			udp_msg_type = buf_cpy[0] ; 
		//	printf("data is: %s\n", data) ; 

			unsigned short final_dst = buf_cpy[1] << 8 | buf_cpy[2];
			int next_hop ; 
			next_hop = whatIsNextHop(final_dst, routingTable, numRoutes) ; 

			if( next_hop  == -1) {
				//there is no entry for this destination in the table;

				waitMore = 1 ; 
				counterRoute ++ ; 
				if (counterRoute == 10000) {

					printf("packet dropped, Data: %s\n", data);
					sprintf(msg_arr, "DROP %s\n",data  ) ;
					if ( send(sockfd, msg_arr ,strlen(msg_arr), 0) != strlen(msg_arr) )  
					{
						perror("Could not send FWD LOG successfuly :( ");
						exit(1);
					}
//					counterRoute = 0 ;
					waitMore = 0 ; 					
				}

			 
			}
			else 
			{
				waitMore = 0 ; 
				printf("next_hop is %d\n", next_hop ) ; 

				char next_hop_add_char[20] ; 
				sprintf(next_hop_add_char, "%d", next_hop ) ; 
				node * next_hop_node = find(start, next_hop_add_char) ; 
						//printf("UDP msg, type 1, final dst = %d - %c - %c - %s - %d\n", final_dst, buf_cpy[1], buf_cpy[2], data, numbytes) ; 		


				//send the log message to manager
				printf("FWD %d %s %s\n",final_dst, next_hop_node->addr, data  ) ; 
				sprintf(msg_arr, "LOG FWD %s %s\n",next_hop_node->addr, data  ) ;
				if ( send(sockfd, msg_arr ,strlen(msg_arr), 0) != strlen(msg_arr) )  
				{
					perror("Could not send FWD LOG successfuly :( ");
					exit(1);
				}
				get_command(sockfd, msg_arr, "LOG OK\n"); // manager sends LOG OK message
				

				//forward the packet to the next hop
				if ((numbytes = sendto(next_hop_node->fd, buf_cpy, sizeof(buf_cpy), 0, next_hop_node->p.ai_addr, next_hop_node->p.ai_addrlen)) == -1) 
				{
					perror("Packet could not be forwarded to the next hop!\n");
					exit(1);
	 			}

			}


			
		}


		numbytes = recvfrom(my_node.fd , buf, MAXBUFLEN-1 , 0,(struct sockaddr *) 0, (int *) 0);

	    	if ( numbytes > 0)
		{
//			sprintf(buf_tmp,"%s", buf); 
//			sprintf(data, "%s", &buf[3]);
			strcpy(data,&buf[3]) ; 
			unsigned char udp_msg_type ; 
			udp_msg_type = buf[0] ; 


			if (udp_msg_type == 1 )		//this is a data message
			{
				printf("data is: %s\n", data) ; 

				unsigned short final_dst = buf[1] << 8 | buf[2];
				unsigned char c1 = buf[1] ; 
				unsigned char c2 = buf[2] ; 

				if(final_dst == atoi(my_node.addr))
				{
					//tell the manager message received!
					printf("recieved the packet!");
					char temp[1024];
					sprintf(temp, "RECEIVED %s\n", &buf[3] ) ;
					if( send(sockfd, temp, strlen(temp), 0) != strlen(temp) ) 
					fprintf(stderr, "client: failed to send Received\n");
					get_command(sockfd, buf, "OK\n"); // Manager confirms message received
					printf("%s", buf);
					printf("received OK; this is final destination\n");
				}
				else
				{ 
/*
				int j ; 
				printf("start\n");
				for ( j = 0 ; j < numRoutes ; j ++ ) 
				printf("Routing Table: ad:%s cost:%d NH:%s\n" , routingTable[j].addr,routingTable[j].cost, routingTable[j].nextHop);
*/
					int next_hop ; 
					next_hop = whatIsNextHop(final_dst, routingTable, numRoutes) ; 

					if( next_hop  == -1) {

						//there is no entry for this destination in the table;

unsigned char c0 = 1 ; 
 
						sprintf(buf_cpy,"%c%c%c%s\0",c0,c1,c2,data) ; 
						printf("waitmore = 0 ; buf: %s\n", buf_cpy) ; 
						waitMore = 1 ;  

					}
					else
					{
						waitMore = 0 ; 

						printf("next_hop is %d\n", next_hop ) ; 

						char next_hop_add_char[20] ; 
						sprintf(next_hop_add_char, "%d", next_hop ) ; 
						node * next_hop_node = find(start, next_hop_add_char) ; 
						//printf("UDP msg, type 1, final dst = %d - %c - %c - %s - %d\n", final_dst, buf[1], buf[2], data, numbytes) ; 		


						//send the log message to manager
						printf("FWD %d %s %s\n",final_dst, next_hop_node->addr, data  ) ; 
						sprintf(msg_arr, "LOG FWD %s %s\n",next_hop_node->addr, data  ) ;
						if ( send(sockfd, msg_arr ,strlen(msg_arr), 0) != strlen(msg_arr) )  
						{
							perror("Could not send FWD LOG successfuly :( ");
							exit(1);
						}
						get_command(sockfd, msg_arr, "LOG OK\n"); // manager sends LOG OK message
					

						//forward the packet to the next hop
						if ((numbytes = sendto(next_hop_node->fd, buf, sizeof(buf), 0, next_hop_node->p.ai_addr, next_hop_node->p.ai_addrlen)) == -1) 
						{
							perror("Packet could not be forwarded to the next hop!\n");
							exit(1);
	 					}
					}

				}
			}

			if (udp_msg_type == 3) 
			{
				//printf("control msg: %s\n", &buf[1]);
				char address[20], costArr[20] ;  
				char senderAddr[20] ; 
				char pathRCV[PATHLENGTH][20] ; 
				int p ; 
				for(p = 0 ; p < PATHLENGTH ; p ++)
					strcpy(pathRCV[p],"E");

				//msg is a control type; 
				strcpy(senderAddr, strtok ( &buf[2], ", " ) ); 
				node * nextHop = find(start, senderAddr) ;

				strcpy(address, strtok ( NULL, ", ;" )) ;
				strcpy(costArr, strtok ( NULL, ", ;" )) ;


				while (strcmp(address, "-2") != 0)
				{		

					int j ; 
					for(j = 0 ; j < PATHLENGTH ; j++) {
						strcpy(pathRCV[j],strtok ( NULL, "; ," ) ); 

				//		printf("path: %s\n",pathRCV[j]);
						if(strcmp(pathRCV[j],"E") == 0)
							break ;

					}

					
					int costRCV = atoi(costArr) ;
					int k ;

					//address
					//nextHop
					//senderAddr

					for(k =0 ; k < numRoutes ; k++ )
					{

				//		printf("k:%d address: %s costRCV: %d nextHopCost:%d\n" , k,  address,costRCV,nextHop->cost) ;
				//		printf("RTaddr: %s RTcost:%d\n",routingTable[k].addr, routingTable[k].cost);
						 				
						if (strcmp(address, routingTable[k].addr) == 0 )
						{
					//	printf("passed!\n");

											
							if(costRCV == -1)
								if(strcmp(routingTable[k].nextHop, senderAddr) == 0) {
								{
									routingTable[k].cost = -1 ;
									
									
								}
							break ;

							}

							else if(routingTable[k].cost == -1 && (costRCV + nextHop->cost > 0)) {

								int flag1 = 0, i ; 
								for(i = 0 ; i < PATHLENGTH ; i++){
									if(strcmp(pathRCV[i], my_node.addr) == 0 ) 
										flag1 = 1 ; 
								}



								if ( flag1 == 0 )
								{
									strcpy(routingTable[k].nextHop, senderAddr);
									routingTable[k].cost = costRCV + nextHop->cost ;
									int j ; 
									for (j = 0 ; j < PATHLENGTH ; j++){
										strcpy(routingTable[k].path[j],pathRCV[j]);
									}
								}
							break ; 
							}

							else if (costRCV + nextHop->cost <= routingTable[k].cost) 
							{



								int flag = 0, i ; 

								for(i = 0 ; i < PATHLENGTH ; i++){
									printf(" %d ", i ) ; 
									printf("%s", pathRCV[i]) ; 


									if(strcmp(pathRCV[i], my_node.addr) == 0 ) {
										flag = 1 ; 
										
									}
								} 

								printf("update if not on path flag : %d, %s, %d", flag, address, costRCV);

								if ( flag == 0 )
								{
									strcpy(routingTable[k].nextHop, senderAddr);
									routingTable[k].cost = costRCV + nextHop->cost ;
									int j ; 
									strcpy(routingTable[k].path[0],senderAddr);
									for (j = 0 ; j < PATHLENGTH-1 ; j++){
										strcpy(routingTable[k].path[j],pathRCV[j]);
									}
								}
							break ; 

							}

							break ;	  
						}
					}


					if (k == numRoutes) {	//this is a completely new route
						routingTable[k].cost = costRCV + nextHop->cost ; 
						strcpy(routingTable[k].nextHop, senderAddr) ; 
						strcpy(routingTable[k].addr , address ) ; 
						numRoutes ++ ; 
						
					}

				strcpy(address, strtok ( NULL, ", ;" )) ;
				strcpy(costArr, strtok ( NULL, ", ;" )) ;
						
				}
/*
	int j ; 
	printf("start\n");
	for ( j = 0 ; j < numRoutes ; j ++ ) 
		printf("Routing Table: ad:%s cost:%d NH:%s\n" , routingTable[j].addr,routingTable[j].cost, routingTable[j].nextHop);

*/
			}	
	   	}


		// For TCP msgs 


	//	if (FD_ISSET(sockfd, &readfds)) {
		
		if (recv(sockfd, buf, 100, MSG_DONTWAIT) > 0 )		
		{	
	//			printf("new TCP messaage!\n") ; 
			strcpy(msg_arr, strtok(buf,"\n")) ; 
			strcpy(temp_msg_arr, strtok(msg_arr," \n")) ; 
//				printf("%s\n",temp_msg_arr) ; 

				// Check for end of simulation
			if ( strcmp(temp_msg_arr, "END") == 0) {
				if( send(sockfd, "BYE" , strlen("BYE"), 0) != strlen("BYE") ) 
					fprintf(stderr, "client: failed to send cost OK\n");
				close(sockfd); //clean up the socket file descriptor
				return 0;
			}	

	 		// Check for link cost event
			if ( strcmp(temp_msg_arr, "LINKCOST") == 0) {

				char node1[5], node2[5], cost_arr[5] ;
				int cost_int ; 

	 			strcpy(node1, strtok(NULL," \n")) ; 
				strcpy(node2, strtok(NULL," \n")) ; 
				strcpy(cost_arr, strtok(NULL," \n")) ; 
				cost_int = atoi(cost_arr) ; 

				printf("Node 1: %s\n", node1);
				printf("Node 2: %s\n", node2);
				printf("My Address: %s\n", my_node.addr);
				printf("Cost: %d\n", cost_int);

					//printf("none changed!");

				if ( strcmp(node1 , my_node.addr) == 0 )
				{
					printf("node2 changed! ") ; 
					node_to_be_changed = find(start, node2) ;
					node_to_be_changed->cost = cost_int ; 

						//change in the routing table
					int k ;

					for (k = 0 ; k < numRoutes ; k++){
						if (strcmp (routingTable[k].addr, node2)==0) 
							routingTable[k].cost = cost_int ; 
					}
				}
				else if (strcmp(node2 , my_node.addr) == 0) 
				{
					printf("node1 changed! ") ; 
					node_to_be_changed = find(start, node1) ;
					node_to_be_changed->cost = cost_int ; 
					int k ; 
					for (k = 0 ; k < numRoutes ; k++){
						if (strcmp (routingTable[k].addr, node1) == 0  ) 
							routingTable[k].cost = cost_int ; 
					}
				}	
				else
				{
					printf("none changed! ");
				}



				sprintf(temp_msg_arr, "COST %d OK\n", cost_int) ; 
				printf("%s",temp_msg_arr) ;
	   			if( send(sockfd, temp_msg_arr , strlen(temp_msg_arr), 0) != strlen(temp_msg_arr) )
				{

					fprintf(stderr, "client: failed to send cost OK\n");

//At this point broadcast the link cost change
				}
				broadcastRT(routingTable, start, &my_node ) ;



			}


			
		}
		//}


	}

	return 0;
}


/* get_in_addr - Helper function to fill sockaddr struct
 */

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

	/* Retrieves a command from the server. Guaruntees a complete read
	 *
	 * sockfd: file descriptor for the socket we will retrieve the message from
	 * msg_arr: destination array for the command string
	 * sentinel: a string that indicates the end of the message
	 */
	int get_command(int sockfd, char * msg_arr, char * sentinel)
	{
		int received;
		int i = 0;
		int sentinel_len = strlen(sentinel);

		while (1)
		{
			if((received = recv(sockfd, &msg_arr[i], MAX_COMMAND_SIZE, 0)) != 0)
			{
				i = i + received;
				msg_arr[i] = '\0'; // ensures that our call to strcmp will terminate gracefully
				if( i >= sentinel_len )
				{
				printf("%s", msg_arr) ;
					if(strcmp(&msg_arr[i-sentinel_len], sentinel) == 0)
					{
						break;
					}
				}
			}
		}
		return 0;
	}



int udp_init(node* const start, node * my_node) {

	// setup the UDP connection for the immediate neighbors

	node* neighbor ; 
	neighbor = start->next ; 

	while (neighbor != NULL ) {
	
		single_udp_setup (neighbor) ; 
		neighbor = neighbor->next ; 

	}


	//setup my own FD and UDP port 

	   int sockfd;
	    struct addrinfo hints, *servinfo, *p;
	    int rv;
	    int numbytes;
	    struct sockaddr_storage their_addr;
	    char buf[MAXBUFLEN];
	    socklen_t addr_len;
	    char s[INET6_ADDRSTRLEN];

	    memset(&hints, 0, sizeof hints);
	    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	    hints.ai_socktype = SOCK_DGRAM;
	    hints.ai_flags = AI_PASSIVE; // use my IP

	    if ((rv = getaddrinfo(NULL, my_node->udpport, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	    }

	    // loop through all the results and bind to the first we can
	    for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
		        p->ai_protocol)) == -1) {
		    perror("listener: socket");
		    continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
		    close(sockfd);
		    perror("listener: bind");
		    continue;
		}

		break;
	    }

	    if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	    }

	    freeaddrinfo(servinfo);

		my_node->p = *p ; 
		my_node->fd = sockfd ; 

	return 0 ; 
	
}


int single_udp_setup(node* neighbor ) {
		//setup a UDP connection with the neighbor whose information is saved in node* neighbor

	    int sockfd;
	    struct addrinfo hints, *servinfo, *p;
	    int rv;
	    int numbytes;


	    memset(&hints, 0, sizeof hints);
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_DGRAM;

	    if ((rv = getaddrinfo( neighbor->hostname, neighbor->udpport , &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	    }

	    // loop through all the results and make a socket
	    for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
		        p->ai_protocol)) == -1) {
		    perror("talker: socket");
		    continue;
		}

		break;
	    }

	    if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	    }

	
		neighbor->fd = sockfd ; 
		neighbor->p = *p ; 

	return 1; 
}

int whatIsNextHop(int final_dst, nodeRT* routingTable, int numRoutes) {
	//returns next hop for this final destiantion 
	//returns -1 if the next hop for this final destination does not exist.

	int j = 0 ; 

	for (j = 0 ; j < numRoutes ; j++ ) 
	{
		if (final_dst == atoi(routingTable[j].addr) && routingTable[j].cost!= -1)
			return atoi(routingTable[j].nextHop) ; 
	}
	
	if ( j == numRoutes) 
		return -1; 


}

int max(int a , int b) {

	if (a >b ) 
		return a ; 
	else 
		return b ; 

	
}

void setnonblocking(int sock)
{
	int opts;

	opts = fcntl(sock,F_GETFL);
	if (opts < 0) {
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sock,F_SETFL,opts) < 0) {
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
	return;
}


void broadcastRT(nodeRT* routingTable, node* pointer, node * my_node){
//Broad cast the routing table to the immediate neighbors
	
 

	char RTcontent[MAXRTCONTENTPACKETSIZE] ;	
	//this is the type for control messages
	//first byte is 3
	//then is the address of the sender
	//infomration are space separated
//	printf("boradcast started!\n") ;
//	printf("numrutes: %d\n", numRoutes);
	RTcontent[0] = 3 ;
	RTcontent[1] = ' ' ; 
	strcpy(&RTcontent[2], my_node->addr); 


	int i = 0 ; 
	for ( i = 0 ; i < numRoutes ; i ++ ) 
	{
		//sprintf(singleRoute, "%s;%d", routingTable[i].addr, routingTable[i].cost) ; 
		sprintf(RTcontent, "%s %s,%d", RTcontent,routingTable[i].addr, routingTable[i].cost ) ;
		int k ;
		for ( k = 0 ; k < PATHLENGTH ; k++ ) 
		{
			sprintf(RTcontent, "%s;%s", RTcontent,routingTable[i].path[k] ) ;
			if (strcmp(routingTable[i].path[k],"E") == 0 )
				break ; 

		}
//		printf("step: %s\n", RTcontent) ; 

	}
	sprintf(RTcontent, "%s -2,-2", RTcontent) ;
//	printf("RTpacket made\n"); 
//	printf("RTcontent: %s\n", RTcontent) ; 

	pointer =  pointer -> next; //First node is dummy node.
 

        while(pointer!=NULL)
        {
		//send the packet
		int numbytes = 0 ;
//		printf("%s cost: %d\n", pointer->addr, pointer-> cost) ; 
		

		if (pointer->cost != -1)		
		{
			if ((numbytes = sendto(pointer->fd, RTcontent, sizeof(RTcontent), 0, pointer->p.ai_addr, pointer->p.ai_addrlen)) == -1) 
			{
				perror("control message coul not be sent to the immediate neighbor!\n");
				exit(1);
			}					

		}	
                pointer = pointer -> next;//Search in the next node.
        }

//	printf("after while!\n");3 cost: 0


	return ; 

}



int isInThePath( char ** path, char* hop) {
	//returns 1 if the hop is in the path
	//return 0 if not in the path
	int i ;
	printf("inside isitthepath") ; //: %s %s" , hop , path[0], path[1] ) ;  
	for(i = 0 ; i < PATHLENGTH ; i++){
		if(strcmp(path[i], hop) == 0 ) 
			return 1 ; 
	}

	return 0 ; 
}

