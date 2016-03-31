#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "Retreive.h"

#define LoggingOn 1
#define ECHOMAX 255     /* Longest string to echo */


void DieWithError(char *errorMessage);  /* External error handling function */

int sendBook(char *servIP,unsigned short echoServPort, int sock, const struct ClientMessage echoStruct);

int sendLogin(char *servIP,unsigned short echoServPort, int sock, const struct loginMsg TCPID);

int IsValidIsbn13(char *isbn);


int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    struct ClientMessage echoStruct; /* String to send to echo server */
	struct loginMsg TCPID;			 /* Struct to send the Login Credentials to the server*/
	struct ServerMessage serstructecho;/*Recieved struct from server side*/
	char *requestString; 			 /* Enum String for output echoStruct.requesttype*/
	char *respTypeString;
	char *ClientRqtype;				 /* Request type specified by the user Q/B/R */
    char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
    int echoStructLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */
	int recvMsgSize;
	char inputkey;
	int UserID;
	int UniquereqID = 0;
	int Exitflag = 0;				 /*Exit flag is set to 0 but if the user enters an X it will exit the program*/
	int Validflag = 1;				 /*Validation for the operations to make sure it is B/Q/R/X */
	
    printf("--------------------4444444--------------------------------------------\n");

	
	TCPID.UserID = 3279;                                	 /* unique client identifier */
	TCPID.idok = 1;  						/* same size as an unsigned int */                             
	TCPID.TCPPort = 2727;
	
	
    if ((argc < 2) || (argc > 3))    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
        exit(1);
    }
    
    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]);  /* Use given port, if any */
	
	for(;;){
		if(Validflag == 1){
			printf("----------------------------------------------------------------\n");
			printf("\n \n Welcome to the talk session. \n");
			printf(" First this client will automatically send your TCP Port and UserID to the server \n");
			printf(" The server will respond with a Success/Failure message \n");
			printf("----------------------------------------------------------------\n");
		}
		
		
		/* Create a datagram/UDP socket */
			if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
				DieWithError("socket() failed");
		
			if(LoggingOn == 1){
			printf(" about to echo string %s \n",echoStruct.isbn);
			printf(" about to echo rectype %d \n",echoStruct.requestType);
			}
			
			sendLogin(servIP,echoServPort,sock,TCPID);
			
			respStringLen = sizeof(serstructecho);
		
		/* Recv a response */
			fromSize = sizeof(fromAddr);
				if ((respStringLen = recvfrom(sock, (char*) &serstructecho, respStringLen, 0,
					 (struct sockaddr *) &fromAddr, &fromSize)) != respStringLen)
					DieWithError("recvfrom() failed");

		//Query Case Returns Book details		
			
	}
	close(sock);
	exit(0);
}
	




 int sendBook(char *servIP,unsigned short echoServPort, int sock, const struct ClientMessage echoStruct)
{
	size_t echoStringLen2 = sizeof(echoStruct);
	    /* Construct the server address structure */
	struct sockaddr_in echoServAddr; /* Echo server address */
	
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort);     /* Server port */
	
	if(LoggingOn == 1)
	printf(" Checking echo string %d \n",echoStruct.requestID);
    
    /* Send the string to the server */
    if (sendto(sock, (char*) &echoStruct, echoStringLen2, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != echoStringLen2)
        DieWithError("sendto() sent a different number of bytes than expected");
		
	//printf(" Sending string %d \n",echoStruct.requestType);
}

int sendLogin(char *servIP,unsigned short echoServPort, int sock, const struct loginMsg TCPID)
{
	size_t loginMsgLen = sizeof(TCPID);
	    /* Construct the server address structure */
	struct sockaddr_in echoServAddr; /* Echo server address */
	
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort);     /* Server port */
	
	if(LoggingOn == 1)
	printf(" Checking echo string %i \n",TCPID.TCPPort);
    
    /* Send the string to the server */
    if (sendto(sock, (char*) &TCPID, loginMsgLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != loginMsgLen)
        DieWithError("sendto() sent a different number of bytes than expected");
		
	//printf(" Sending string %d \n",echoStruct.requestType);
}



// Validate isbn return 1 if valid and 0 if invalid
int IsValidIsbn13(char *isbn)
    {
        int result = 0;
        

		// Comment Source: Wikipedia
		// The calculation of an ISBN-13 check digit begins with the first
		// 12 digits of the thirteen-digit ISBN (thus excluding the check digit itself).
		// Each digit, from left to right, is alternately multiplied by 1 or 3,
		// then those products are summed modulo 10 to give a value ranging from 0 to 9.
		// Subtracted from 10, that leaves a result from 1 to 10. A zero (0) replaces a
		// ten (10), so, in all cases, a single check digit results.
		int sum = 0;
		int i = 0;
		for (i = 0; i < 12; i++)
		{
			sum += (isbn[i]-'0') * (i % 2 == 1 ? 3 : 1);
		}

		int remainder = sum % 10;
		int checkDigit = 10 - remainder;
		if (checkDigit == 10) checkDigit = 0;

		result = checkDigit == (isbn[12]-'0');

		if(LoggingOn == 1)
		printf("isbn checkdigit : %d\n",checkDigit);
        return result;
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
	
	




