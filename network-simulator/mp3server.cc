#include "mp3server.hh"
#include "mp3.cc"
#include "linkedlist.cc"


uint64_t GetTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
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


void new_client(connection listener, char * input_file_name)
{
// create new thread per client
  FILE * fp ; 
  fp = fopen (input_file_name, "r") ; 
  
  char buf_send [MAXPACKETSIZE+1] ; 
  char buf_rec [MAXPACKETSIZE+1] ; 
  
  size_t  numBytesRead, numbytesSent ;


  
  //initiazle the linkedlist
        node *start,*temp;
        start = (node *)malloc(sizeof(node)); 
        temp = start;
        temp -> next = NULL;
  
  /*
  the packet structure is like this:
  packet[0] : i , a, e, d
  
  i: when the client is initializing the connection with server.
  
  a: this is an ack packet!
    packet[2,3,4,5]: packet ID (this is a long integer)!
  
  e: this is end of data packet!
  
  d: this is a data packet!
    packet[1]: number of data bytes!
    packet[2,3,4,5]: packet ID (this is a long intereger)!
    packet[6,7]: empty!
    packet[8 ... 99]: contain data. ( 92 bytes)
  
  */
  
  int PID = 0 ; 
  char PIDArr[4] ; 
  
  int cwnd = 1 ; 
  int ssthresh = 65535;
  
  //this keeps track of the ID of the last received ACK packet!
  long int lastACKRec = 0 ;
  long int lastLastACRrec = 0 ;
  int EOF_flag = 0 ; 
  int fastRetrans_flag = 0;
  
  int dupACK = 0 ;
  
  time_t rto = TIMEOUTINIT ; 
  
  //sets the settings for the socket to be non blocking!
  setnonblocking(listener.sockfd);
  
	while (1) // begin sending the file over TCP
	{

		buf_rec[0] = '\0' ; 
		//check if there is any packet?
		recvfrom(listener.sockfd, buf_rec, MAXPACKETSIZE , 0, (struct sockaddr *)&listener.their_addr, &listener.addr_len );
		//a packet is received from client!
		
			node* expiredNode = NULL ; 
			 
			
			if ( (expiredNode = timeExpCheck(start, GetTimeStamp() ) ) != NULL)
			{
			  //do retransmit!
			    //printf("expired node is %lu ", expiredNode->PID) ;
// 			    //printf("cwnd: %d, ssthresh: %d\n", cwnd, ssthresh) ; 
			      expiredNode->expTime = GetTimeStamp() + rto ; 
			      printf("rto: %lu", rto);
			      expiredNode->retrans = 1 ; 

			      numbytesSent = mp3_sendto(listener.sockfd,
				expiredNode->data, expiredNode->dataSize + HEADERSIZE,
				0, (struct sockaddr *)&listener.their_addr,
				listener.addr_len); 
			      
			      if( numbytesSent == -1 )
			      {
				perror("sendto");
				exit(1);
			      }
			      
			  
			  //reset the valuse of cwnd and ssthresh
			  ssthresh = max(cwnd/2, 2) ; 
			  cwnd = 1;  
			  
			  printf("node Exp: cwnd: %d, ssthresh: %d\n", cwnd, ssthresh) ; 
			}
		
		
		if (buf_rec[0] == 'a') // ack packet
		{
			printf("cwnd: %d, ssthresh: %d\n", cwnd, ssthresh) ; 
			int PID_rec = (
					(buf_rec[2] << 24)&(0xFF000000)
				      | (buf_rec[3] << 16)&(0x00FF0000) 
				      | (buf_rec[4] << 8 )&(0x0000FF00)
				      | (buf_rec[5]) & (0X000000FF)
				      ) ; 
			buf_rec[0] = '\0' ;					  
			printf("ack received with PID : %u === %x %x %x %x\n", PID_rec, buf_rec[2],buf_rec[3],buf_rec[4],buf_rec[5] );
			

			//TODO set RTO RTO = a*NEW_RTO + (1-a)*RTO ; 
			
			//find time expirations!

			time_t time = GetTimeStamp() ; 
			//printf("%lu\n", time) ; 
			//iterate(start) ;
			printf("\n\n") ;
			

			if(PID_rec == lastACKRec && fastRetrans_flag == 0)
			{
			  //DUPACKS
			  dupACK ++ ; 
			}
			else 
			  dupACK = 0 ;

			
			if(dupACK == 2)
			{
			  //if there are two dupACKS
			  
			  printf("dupACKs detected!\n");
			
			  node* thisPacket = find(start, PID_rec) ;
			  thisPacket->retrans =1 ; 
			  if (thisPacket != NULL)
			  {
			    numbytesSent = mp3_sendto(listener.sockfd, thisPacket->data, thisPacket->dataSize + HEADERSIZE, 0, (struct sockaddr *)&listener.their_addr, listener.addr_len); 
			    if( numbytesSent == -1 )
			    {
			      perror("sendto");
			      exit(1);
			    }
			    
			  }
			  
			  
			  
			  
			  // SET the ssthresh to half the cwnd due to dupACKs
			  ssthresh = max(cwnd / 2, 2)  ; 
			  cwnd = max(2,cwnd/2) ;
			  
			  
			  
			  dupACK = 0 ; 
			  fastRetrans_flag = 1 ; 
			}
			else
			  fastRetrans_flag = 0 ; 
			
			

			
			if (PID_rec > lastACKRec)
			{
				lastLastACRrec = lastACKRec ; 
				lastACKRec = PID_rec ;
				
				
			}
			
			long int k = 0 ; 
			node* lastNode ; 

			for (k = lastLastACRrec ; k < lastACKRec ; k ++) {
			  
			  
			  if(k == lastACKRec -1) 
			  {

			    if( (lastNode = find(start, k)) != NULL)
			    {

			      time_t a ; 
			      time_t delta;
			      
			      if(lastNode->retrans == 0)
			      {
				
				delta = (GetTimeStamp() - lastNode->insertTime)*(1024-ALPHA) ;
				a = ( rto ) * ALPHA ; 
				rto = a + delta ; 
				rto = rto >> 10 ; 
				printf("rto: %lu %lu %lu\n", rto, a , delta);
			      }
			      
			    			      
			    }
			      

			  }
			  
			  deleteNode(start, k);  
			  
			  
			}
			

			
			// slow start - double the window
				// regular operation (cwnd > ssthresh) - additive increase
			if (cwnd < ssthresh)
			  cwnd = cwnd*2;
			else
			  cwnd ++ ; 

			printf("cwnd: %d, ssthresh: %d\n", cwnd, ssthresh) ; 	  
		}
      
		if(listSize < cwnd && EOF_flag == 0 ) 
		{
		  printf("cwnd: %d, ssthresh: %d\n", cwnd, ssthresh) ; 
			printf("listSize: %d, cwnd: %d\n", listSize, cwnd) ; 
			//as long as number of packets in the out_buffer are smaller
			//than the maximum size of the buffer, read from file and create 
			//packets.

			// read the next block of character from the file
			int i= 0 ;
			for(i = HEADERSIZE ; i < MAXPACKETSIZE; i++)
			{
				int next = fgetc(fp);
				if( next == EOF )
				{
					// done, stop here
					EOF_flag = 1 ; 
					break ; 

				}
				else
				{
					// everything ok
					buf_send[i] = (char)next ;
					
				}
			}
			numBytesRead = i-HEADERSIZE ; 
			
			printf("number of bytes read: %d\n", (int)numBytesRead) ; 
			

			node * newNode = get_new_node() ;

			newNode->PID = PID ; 
			newNode->dataSize = (int)numBytesRead ; 

			buf_send[0] = 'd' ; 
			
			unsigned char temp_PID[4] ; 

			
			temp_PID[0] = (int)((PID >> 24) & 0xFF) ;
			temp_PID[1] = (int)((PID >> 16) & 0xFF) ;
			temp_PID[2] = (int)((PID >> 8) & 0XFF);
			temp_PID[3] = (int)((PID & 0XFF));

    			
			
			
			buf_send[2] = temp_PID[0] ;  
			buf_send[3] = temp_PID[1] ;  
			buf_send[4] = temp_PID[2] ; 
			buf_send[5] = temp_PID[3] ;  

			unsigned char temp_size[2] ; 
			
			
			temp_size[0] = numBytesRead ;  
			temp_size[1] = numBytesRead >> 8 ;  
			
			buf_send[6] = temp_size[0] ; 
			buf_send[7] = temp_size[1] ; 
			
			int p = 0 ; 
			for (p = 0 ; p < numBytesRead + HEADERSIZE ; p++)
			  newNode->data[p] = buf_send[p] ; 
			
			// set expiration time for this packet

			newNode->expTime = GetTimeStamp() + rto ;
			
			printf("exptIme : %lu, curtime: %lu", newNode->expTime, GetTimeStamp()); 
			newNode->retrans = 0 ; 
			//printf("%lu, %d, %lu", rto, GetTimeStamp(), rto + GetTimeStamp()) ;
			newNode->insertTime = GetTimeStamp() ; 
			
			insert(start, newNode) ; 

			PID++ ;		

			numbytesSent = mp3_sendto(listener.sockfd, newNode->data, numBytesRead + HEADERSIZE, 0, (struct sockaddr *)&listener.their_addr, listener.addr_len); 
			if( numbytesSent == -1 )
			{
			  perror("sendto");
			  exit(1);
			}
			
			//printf("read buf(DATA) : %s\n", &(newNode->data[HEADERSIZE]) );
			printf("PID: %d\n", PID) ; 
			printf("%d bytes sent!\n\n\n", (int)numbytesSent) ; 
			printf("cwnd: %d, ssthresh: %d\n", cwnd, ssthresh) ; 
   		}	
   		
   		
   		// if all the packets are sent out, braek!
   		if (listSize == 0 && EOF_flag == 1 ) 
		{

		  break ;

		}
		
		//usleep(100000) ; 
	}
      
  
  // when all the packets are sent, send the last packet and finish the communication
	printf("send e!\n");

	buf_send[0] = 'e' ; 
	buf_send[1] = '\0' ; 

	int c = 0 ; 

	while(1)
	{
		numbytesSent = mp3_sendto(listener.sockfd, buf_send, 100, 0, (struct sockaddr *)&listener.their_addr, listener.addr_len); 
		if( numbytesSent == -1 )
		{
		  perror("sendto");
		  exit(1);
		}
// 
		
		  buf_rec[0] = '\0' ; 
		  //check if there is any packet?
		  recvfrom(listener.sockfd, buf_rec, MAXPACKETSIZE , 0, (struct sockaddr *)&listener.their_addr, &listener.addr_len );
		  //a packet is received from client!
		  
		  if(buf_rec[0] == 'a' && buf_rec[2] == 'e' && buf_rec[3]=='e' && 
		    buf_rec[4]=='e' && buf_rec[5] == 'e')
		  {

		    
		    break ; 		    
		  }
	    
		
		
	}
  
	fclose(fp) ; 
}




int main( int argc, char *argv[] )
{
	if(argc < 2)
	{
		printf("usage: mp3server <server port number> <input file name>\n");
		return -1;
	}

	mp3_init() ; 
	
	char * port = argv[1]; // port used for sending the file
	char * input_file_name = argv[2]; // name of the file to send

	printf("port: %s \n", port);
	printf("filename: %s \n", input_file_name);

	connection listener ; 
	
	if(init_udp(port, &listener) != 0) // initialize UDP port server will listen on
	{
		printf("could not bind UDP socket \n");
		return -1;
	}
  
  	printf("listener: waiting to recvfrom...\n");

	char buf[MAXPACKETSIZE];
	int numbytes;

	// This function is called before the fd is set to non-blocking
	if ((numbytes = recvfrom(listener.sockfd, buf, MAXPACKETSIZE-1 , 0,
		(struct sockaddr *)&listener.their_addr, &listener.addr_len)) == -1) 
	{
		perror("recvfrom");
		exit(1);
	}	
	
	char s[INET6_ADDRSTRLEN];
	printf("listener: got packet from %s\n", inet_ntop(listener.their_addr.ss_family,			get_in_addr((struct sockaddr *)&listener.their_addr),			s, sizeof s));
	printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';
//	printf("listener: packet contains \"%s\"\n", buf);

	

//waiting until receiving the initializing packet from client!
//it starts with i
	if (buf[0] = 'i') 
	{
		printf("connection init\n"); 
	    
		//send ack back
	    
		buf[0] = 'a' ;

		if (mp3_sendto(listener.sockfd, buf, MAXPACKETSIZE, 0, (struct sockaddr *)&listener.their_addr, listener.addr_len) == -1 ) 
		{
		perror("sendto");
		exit(1);
		}

		new_client(listener, input_file_name) ; 
	}

	close(listener.sockfd);	
	
	return 0;
}



















// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int init_udp(char * port, connection * listener)
{
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char buf[MAXPACKETSIZE];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((listener->sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(listener->sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(listener->sockfd);
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

	listener->addr_len = sizeof (listener->their_addr);

	return 0;
}
