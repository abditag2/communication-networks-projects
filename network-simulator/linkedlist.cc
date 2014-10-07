#include<stdio.h>
#include<stdlib.h>

int listSize = 0 ;

#define MAXPACKETSIZE 100 

typedef struct Node 
{
	int PID;
	char data[MAXPACKETSIZE] ; 
	long int expTime ; 
	int numAckRecieved ; 
	int dataSize ;
	int retrans; 
	time_t insertTime ; 
	

        struct Node *next;
}node;




//For the node Linked list


node* get_new_node() 
{

        node* new_node ; 
        new_node = (node*)malloc(sizeof(node)) ;
        return new_node ; 

}


void insert(node *pointer, node* new_node)
{
 /* Iterate through the list till we encounter the last node.*/
	  while(pointer->next!=NULL)
	  {
		  pointer = pointer -> next;
//		  printf("next\n");
	  }

	  pointer->next = new_node ; 
	  pointer = pointer->next;
	  pointer->next = NULL;
	  listSize ++ ; 
}

void iterate(node* start){
  start = start->next ; 
  printf("start\n"); 
   while (start != NULL)
  {
    printf("PID: %d, expTime: %lu\n", start->PID, start->expTime);
    start = start->next ; 
  }
  return;
}

node* find(node *pointer, int key)
{
// 	iterate(pointer) ; 
        pointer =  pointer -> next; //First node is dummy node.
        // Iterate through the entire linked list and search for the key. 
        while(pointer!=NULL)
        {
// //		printf("find %d, %d\n", key, pointer->PID) ; 
                if( key == pointer->PID ) //key is found.
                {
                        return pointer;
                }
                pointer = pointer -> next;//Search in the next node.
        }
        //Key is not found 
        return NULL;
}


void KeepN(node* start, int n)
{
  //keep the first n nodes of linkedlist!
  printf("inside the KeepN listSize: %d, n = %d\n", listSize, n); 
  start = start->next ; 
  int count = 0; 
  node* temp ;
  
  while (start != NULL)
  {
    count++ ; 
    start = start->next ; 
    listSize = count ; 
    printf("count: %d\n", count) ; 
    
    if (count == n)
    {
      while(start->next!= NULL)
      {
	temp = start->next ; 
	start->next = start->next->next;
	free(temp) ; 
      }
      break ; 
    }

  }
  
  
  
  return;


}

node * timeExpCheck(node* pointer, time_t curTime) 
{
// 	iterate(pointer) ; 
	//printf("FArdin!check for expired nodes!\n") ; 
        pointer =  pointer -> next; //First node is dummy node.
        // Iterate through the entire linked list and search for the key. 
        if(pointer!=NULL)
        {
 		
                if( pointer->expTime <= curTime ) //key is found.
                {
		  printf("PID: %d, exptime: %lu, curtime: %lu \n", pointer->PID, pointer->expTime , curTime) ; 
		  return pointer;
			
                }
                else
		  return NULL ;
        }
        //Key is not found 
        return NULL;  
}



void deleteNode(node *pointer, long int key)
{
  
  
        while(pointer->next!=NULL && (pointer->next)->PID != key)
        {
                pointer = pointer -> next;
        }

        if(pointer->next==NULL)
        {
                //printf("Element %lu is not present in the list\n",key);
                return;
        }

        node *temp;
        temp = pointer -> next;
        pointer->next = temp->next;
        free(temp);
	listSize-- ; 
        return;
}


