#include "mp3client.hh"
#include "mp3.cc"
#include "linkedlist.cc"


pthread_mutex_t flag_mutex = PTHREAD_MUTEX_INITIALIZER ; 
pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER ; 



#define _POSIX_X_SOURCE 199309
#define MAXBUFLEN 100

    int updateLinkedList(FILE* fp, node* start, int expectingPID)
    {
      printf("updating linked List, Expecting PID: %d, listSize: %d\n", expectingPID,listSize) ;
      node* nodeInOrder; 
      //find the expecting PID
	printf("node in order!\n");
      
      while( (nodeInOrder = find(start, expectingPID)) != NULL)
      {
	printf("writing to file\n\n\n");
	//int n = fwrite(nodeInOrder->data, 1,nodeInOrder->dataSize, fp);
	
	int f = 0; 
	
	for(f  = 0 ; f < nodeInOrder->dataSize ; f++)
	  printf("%c", nodeInOrder->data[f]);
	  
	int n = fwrite(nodeInOrder->data, 1, nodeInOrder->dataSize, fp);
	printf("n = %d, dataSize: %d\n", n, nodeInOrder->dataSize) ;
	//increase the number by 1, if the expectingPID is found.
	
	deleteNode(start, expectingPID) ;
	expectingPID = expectingPID+1 ; 
	printf("expecting PID: %d\n",expectingPID);
	
      }
      
      return expectingPID; 

    }


int init_udp(char* serverName, char * serverPort, connection * listener)
{
    int sockfd ; 
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(serverName, serverPort, &hints, &servinfo)) != 0) {
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
	
	listener->p = *p ;
	listener->sockfd = sockfd ; 
	
      return 0 ; 
	
}














int main( int argc, char *argv[] )
{
  char buf_rec [100] ;
  char buf_send [100] ;

  mp3_init() ; 
  if(argc != 4)
  {
    printf("usage: mp3client <server name> <server port number> <output file name>\n");
    return -1;
  }


  char * server_name = argv[1];
  char * server_port = argv[2]; // port used for sending the file
  char * filename = argv[3]; // name of the file to send

  connection listener; // structure for storing our connection related info
  if(init_udp (server_name, server_port, &listener) != 0) // initialize UDP port server will listen on
  {
    printf("could not bind UDP socket \n");
    return -1;
  }

  
  
  
  
  
  //send the initialization packet 
   
  while(1) 
  {
  
    buf_send[0] = 'i' ;
    if (mp3_sendto(listener.sockfd, buf_send, sizeof(buf_send), 0,
	    listener.p.ai_addr, listener.p.ai_addrlen) == -1 ) 
    {
      perror("sendto");
      exit(1);
    }
    
    //recieve ack for init packet
    
    recvfrom (listener.sockfd, buf_rec, MAXPACKETSIZE 
      ,0,NULL, NULL) ; 
      
    if (buf_rec[0] == 'a')
      break ; 
    
  }
  
  //initiliaze expecting packet ID
  int expectingPID = 0 ; 
  
  //initiazle the linkedlist
        node *start,*temp;
        start = (node *)malloc(sizeof(node)); 
        temp = start;
        temp -> next = NULL;

//initiliaze the file to write the received data  
  FILE * fp ; 
  fp=fopen(filename,"w");
  int numbytes ; 
  
 
  while( 1) {
    buf_rec[0] = '\0' ; 
    printf("waiting to recvfrom server!\n") ;     

    //recieve a packet!
    numbytes =  
    recvfrom (listener.sockfd, buf_rec, MAXPACKETSIZE,0,NULL, NULL) ; 
    printf("listener: packet is %d bytes long\n", numbytes);
    buf_rec[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf_rec);

    
    
    if (buf_rec[0] == 'd')
    {
      
      			int PID = (
					(buf_rec[2] << 24)&(0xFF000000)
				      | (buf_rec[3] << 16)&(0x00FF0000) 
				      | (buf_rec[4] << 8 )&(0x0000FF00)
				      | (buf_rec[5]) & (0X000000FF)
				      ) ; 
      
      
      unsigned short int dataSize = buf_rec[6]| buf_rec[7]<<8 ; 

      
      
      printf("PID: %d, datasize: %d\n", PID, dataSize);
      
      
      //insert into the linked list
      
      node* newNode = get_new_node(); 
      int p = 0 ; 
      
      for (p = 0; p < dataSize ; p ++)
      {
	newNode->data[p] = buf_rec[HEADERSIZE+p] ; 
      }
      
 
      //printf("packet contains: %s\n" , newNode->data);
      
      newNode->PID = PID ; 
      newNode->dataSize = dataSize ;
	
      if (find(start, newNode->PID) == NULL) 
      {
	insert(start, newNode) ; 
	printf("packet inserted!\n");
      }
      else
	free(newNode);
      
    //  iterate(start) ; 
    }

 
    
    if(buf_rec[0] == 'e')
    {
      printf("e recieved!\n");
      
      buf_send[0] = 'a';
      char ackArray[4]; 
      
      buf_send[2] = 'e';
      buf_send[3] = 'e';
      buf_send[4] = 'e';
      buf_send[5] = 'e';
    
      if (mp3_sendto(listener.sockfd, buf_send, sizeof(buf_send), 0,
	      listener.p.ai_addr, listener.p.ai_addrlen) == -1 ) 
      {
	perror("sendto");
	exit(1);
      }  
      
      break ; 
    }

    //linkedlist update and ack sending step
    
    
    expectingPID = updateLinkedList(fp, start, expectingPID) ; 
    printf("expecting %d\n", expectingPID);

    //prepare the ack pack to send!
     
    
    buf_send[0] = 'a';
    char ackArray[4]; 
    

  ackArray[0] = (int)((expectingPID & 0xFF000000) >> 24 );
  ackArray[1] = (int)((expectingPID & 0x00FF0000) >> 16 );
  ackArray[2] = (int)((expectingPID & 0x0000FF00) >> 8 );
  ackArray[3] = (int)((expectingPID & 0X000000FF)) ;

    

    buf_send[2] = ackArray[0];
    buf_send[3] = ackArray[1];
    buf_send[4] = ackArray[2];
    buf_send[5] = ackArray[3];    
    
      if (mp3_sendto(listener.sockfd, buf_send, sizeof(buf_send), 0,
	      listener.p.ai_addr, listener.p.ai_addrlen) == -1 ) 
      {
	perror("sendto");
	exit(1);
      }  
    printf("ack sent with PID = %d = %x %x %x %x\n", expectingPID, buf_send[5],buf_send[4], buf_send[3],buf_send[2]);
    
    printf("\n\n\n");

  }

  fclose(fp) ; 

  return 0;
}

