#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "Retreive.h"

#define LoggingOn true
#define ECHOMAX 255     /* Longest string to echo */



void DieWithError(char *errorMessage);  /* External error handling function */
int sendBook(char *servIP,unsigned short echoServPort, int sock, const struct ClientMessage echoString);


int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    struct ClientMessage echoString; /* String to send to echo server */
	char *requestString; 			 /* Enum String for output echoString.request type*/
    char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
    int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */
    
    if ((argc < 2) || (argc > 3))    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
        exit(1);
    }
    
    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]);  /* Use given port, if any */
	
    echoString.requestID = 999;       /* Second arg: string to echo */
	echoString.requestType = Borrow;
	strcpy(echoString.isbn,"1234567891234");
    
    if ((echoStringLen = sizeof(echoString)) > ECHOMAX)  /* Check input length */
        DieWithError("Echo word too long");
		
		printf(" Checking echo string \n");
		printf(" port: %d Ip:%s \n",echoServPort,servIP);
    
    
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
	
	printf(" about to echo string %s \n",echoString.isbn);
	
    sendBook(servIP,echoServPort,sock,echoString);
    
    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if ((respStringLen = recvfrom(sock, (char*) &echoString, echoStringLen, 0,
         (struct sockaddr *) &fromAddr, &fromSize)) != echoStringLen)
        DieWithError("recvfrom() failed");
		switch( echoString.requestType ) 
		{
			case 0:
				requestString="Query";
				break;
			case 1:
				requestString="Borrow";
				break;
			case 2:
				requestString="Return";
				break;
			default :
				requestString="fail";
				break;
		}
		
		printf(" Request ID:%d \n",echoString.requestID);
		printf(" Request Type %s \n",requestString);
		printf(" ISBN: %s \n",echoString.isbn);
		
    
    /*if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n");
        exit(1);
    }
	*/
    
    /* null-terminate the received data */
    echoBuffer[respStringLen] = '\0';
   //printf("Received: %s\n", echoBuffer);    /* Print the echoed arg */
    
    close(sock);
    exit(0);
}



 int sendBook(char *servIP,unsigned short echoServPort, int sock, const struct ClientMessage echoString)
{
	size_t echoStringLen2 = sizeof(echoString);
	    /* Construct the server address structure */
	struct sockaddr_in echoServAddr; /* Echo server address */
	
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port   = htons(echoServPort);     /* Server port */
	
	printf(" Checking echo string %d \n",echoString.requestID);
    
    /* Send the string to the server */
    if (sendto(sock, (char*) &echoString, echoStringLen2, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != echoStringLen2)
        DieWithError("sendto() sent a different number of bytes than expected");
		
	printf(" Sending string %d \n",echoString.requestType);
}


 /* Construct the server address structure 
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure 
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family 
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address 
    echoServAddr.sin_port   = htons(echoServPort);     /* Server port 
	
	printf(" Checking echo string %d \n",echoString.requestID);
    
    Send the string to the server 
    if (sendto(sock, (char*) &echoString, echoStringLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
        DieWithError("sendto() sent a different number of bytes than expected");
		
	printf(" Sending string %d \n",echoString.requestType);*/
	
	




