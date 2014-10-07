Team members : 

1)Fardin Abdi Taghi Abad
2)Rick Pospisil

List of Refrences:


For the linked list data structure
http://www.thelearningpoint.net/computer-science/data-structures-singly-linked-list-with-c-program-source-code

for the socket prgramming part:
Beej's instruction provided online

        Our router software is structured as a single-threaded program. On start up, the program performs a handshake with the the manager program to retrieve a list of neighbors. The data structure used is a linked list, where each node stores the address of each neighbor and the necessary information to send a UDP packet to that router. Once the list has been built the program listens on two different sockets: a UDP socket where incoming packets from neighboring routers are received, and a TCP socket which is used to communicate with the manager.
        The algorithm used to make forwarding decisions is a link-state routing algorithm. Our implementation uses Dijkstra's Algorithm to build the routing table. in order to avoid loops, we append the whole path to the message and everytime the node sends out an update to the neighbors, sends oyt the whole path and this way each node will check if it is inside the path or not. If so, there is a loop and will not update its routing table using this new value. This also helps the nodes to have a view from partitions. In this way if a packet is being sent to a node whose link cost is -1 or does not exist, the router waits for some cycles so that maybe in future updates it will hear about this node. if it does not, it will drop the packet.
        To test the software, we used several ring and star networks consisting of between 3 and 10 nodes. Our software performed as expected.
	we have a knwon bug which is if the length of the datapacket is larger than 4 bytes we migh have problems.We tried to solve this but due to time limit we were not successful. 
	


