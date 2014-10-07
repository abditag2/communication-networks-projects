


#include<stdio.h>
#include<stdlib.h>

#define PATHLENGTH 40
typedef struct Node 
{

	char addr[20];
	char hostname[20];
	char udpport[20]; 
	int cost; 
	struct addrinfo p; 
	int fd ; 

        struct Node *next;
}node;


typedef struct NodeRT
{
	char addr[20] ; 
	int cost ; 
	char nextHop[20] ; 
	char path[PATHLENGTH][20] ; 

}nodeRT ; 



//For the node Linked list


node* get_new_node() 
{

	node* new_node ; 
	new_node = malloc(sizeof(node)) ;
	return new_node ; 

}


void insert(node *pointer, node* new_node)
{
        /* Iterate through the list till we encounter the last node.*/
        while(pointer->next!=NULL)
        {
                pointer = pointer -> next;
        }

        pointer->next = new_node ; 
        pointer = pointer->next;
        pointer->next = NULL;
}


node* find(node *pointer, char* key)
{
        pointer =  pointer -> next; //First node is dummy node.
        // Iterate through the entire linked list and search for the key. 
        while(pointer!=NULL)
        {
                if( !strcmp(pointer->addr, key)) //key is found.
                {
                        return pointer;
                }
                pointer = pointer -> next;//Search in the next node.
        }
        //Key is not found 
        return NULL;
}


/**
void delete(node *pointer, int data)
{
        while(pointer->next!=NULL && (pointer->next)->data != data)
        {
                pointer = pointer -> next;
        }

        if(pointer->next==NULL)
        {
                printf("Element %d is not present in the list\n",data);
                return;
        }

        node *temp;
        temp = pointer -> next;
        pointer->next = temp->next;
        free(temp);
           free() will deallocate the memory.
        return;
}

void print(node *pointer)
{
        if(pointer==NULL)
        {
                return;
        }
        printf("%d ",pointer->data);
        print(pointer->next);
}

*/
