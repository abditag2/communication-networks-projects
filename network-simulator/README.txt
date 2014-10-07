TEAM MEMBERS:
Fardin abdi taghi abad
Rick Pospisil

Server:
Upon startup, the server polls the designated UDP port for incoming initializing packets from the client (Initialization packets have the char “i” as the first byte in the header). When this special packet is received, the server sends an ACK and calls new_client(). After an initialization phase, the program enter a while(1) loop where it begins sending the file over our implementation of the TCP algorithm. We used our favorite data structure, a linked list, to store all of our packet data. The structure for each packet is as follows:

PID– Packet id, used to track the acknowledgments as well
data – payload of the packet
expTime – time at which the packet will expire
ackRecieved – truth value, indicates whether we have received an acknowledgment for this packet
dataSize – size of the payload

A variable called “list_size” is used to track the size of the linked list at any given time. This is compared against the congestion window “cwnd” to ensure the number of packets in flight does not exceed the limit set by the TCP algorithm. 

The value of RTO is updated based on the departure time and arrival time of the packets that have not been retransmitted!

Client:
The client is structured similarly to the server. The primary difference is that the client uses the linked list to store packets that have been received out of order and sends the corresponding acknowledgements (reg ack/ dupack) when a data packet is received.

Code was borrowed from beej's guide as well as The Learning Point for implementation of the socket related structures and linked list respectively.
