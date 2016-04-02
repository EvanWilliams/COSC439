#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "Retreive.h"

#define LoggingOn 0
#define ECHOMAX 255     /* Longest string to echo */

void DieWithError(char *errorMessage);  /* External error handling function */
int loadbooks(struct ServerMessage *name);
int findbooks(struct ServerMessage *name, char isbn[13],int numBooks);

struct loginMsg	loggedInUser[20];


 int sock;                        /* Socket */
 
 struct loginMsg	LoginReq;
 
 struct sockaddr_in echoServAddr; /* Local address */
 
 struct sockaddr_in echoClntAddr; /* Client address */
 
 int recvMsgSize;
 
 unsigned int cliAddrLen; 
 

void initClientList(struct loginMsg loggedInUser[])
{
	int i;
	for (i = 0; i < 19; i++){
		loggedInUser[i].UserID = 0;
	}
}	
	
int findLastEntry(struct loginMsg loggedInUser[])
{
	int i;
	for (i = 0; i < 19; i++){
		if( loggedInUser[i].UserID == 0){
			return(i);
		}
		
	}
	return(-1);
}

void addClient(struct loginMsg LoginReq){
	
	extern struct loginMsg loggedInUser[20];
	
	int Client = findLastEntry(loggedInUser);
	if(Client != -1)
	loggedInUser[Client] = LoginReq;
	
}

int findUserID(struct loginMsg loggedInUser[],unsigned int findUserID){
	extern struct loginMsg	NoneSuchFound;
	//prints out the client list if the UserID = Zero then break
	printf("Printing Client List");	
	int i;
	for (i = 0; i < 19; i++){
		
	if( loggedInUser[i].UserID == findUserID)
		return i;
	
	}
	return 0x5555;
}	

void printClientList(struct loginMsg loggedInUser[])
{
	//prints out the client list if the UserID = Zero then break
	printf("Printing Client List");	
	int i;
	for (i = 0; i < 19; i++){
		
	if( loggedInUser[i].UserID == 0)
		break;
	printf("Client #:%d Has a UserId:%d and a TCPPort:%d as well as a idok:%d \n ",i,loggedInUser[i].UserID, loggedInUser[i].TCPPort, loggedInUser[i].idok);
	
	}
}	
int sendLogin(unsigned short echoServPort, int sock, const struct loginMsg TCPID)
{
	size_t loginMsgLen = sizeof(TCPID);
	    /* Construct the server address structure */
	struct sockaddr_in echoServAddr; /* Echo server address */
	
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(INADDR_ANY);  /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort);     /* Server port */
	
	if(LoggingOn == 1)
	printf(" Checking echo string %i \n",TCPID.TCPPort);
    
    /* Send the string to the server */
    if (sendto(sock, (char*) &TCPID, loginMsgLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != loginMsgLen)
        DieWithError("sendto() sent a different number of bytes than expected");
		
	//printf(" Sending string %d \n",echoStruct.requestType);
}

int sendWho(unsigned short echoServPort, int sock, const struct loginMsg loggedInUser[20])
{
	size_t loginMsgLen = sizeof(loggedInUser[20]);
	    /* Construct the server address structure */
	struct sockaddr_in echoServAddr; /* Echo server address */
	
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(INADDR_ANY);  /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort);     /* Server port */

    
    /* Send the string to the server */
    if (sendto(sock, (char*) &loggedInUser[20], loginMsgLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != loginMsgLen)
        DieWithError("sendto() sent a different number of bytes than expected");
		
	//printf(" Sending string %d \n",echoStruct.requestType);
}

void recMessage(){
	 extern int sock;                        /* Socket descriptor */
	 extern struct loginMsg LoginReq;	 /*Recieved struct from server side*/
	 extern int recvMsgSize;               /* Length of received response */
	 extern struct sockaddr_in echoClntAddr;     /* Source address of echo */
	 extern unsigned int cliAddrLen;           /* In-out of address size for recvfrom() */
	
	// Set the size of the in-out parameter 
        cliAddrLen = sizeof(echoClntAddr);
        
		if(LoggingOn == 1)
		printf(" About to receieve LoginMsg \n");
		
        //Block until receive message from a client 
        if ((recvMsgSize = recvfrom(sock, (char*) &LoginReq, ECHOMAX, 0,
                                    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");
        
		
		
}	

int main(int argc, char *argv[])
{
    extern int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    extern struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    struct loginMsg serstructecho;    /* Buffer for echo string */
	struct ServerMessage books[20]; //array of books that are read from the text file
	struct ClientMessage echoStruct; //clientmessage struct
	
	extern struct loginMsg	LoginReq;
	struct loginMsg	Ack;
	struct loginMsg	NoneSuchFound;
	extern struct loginMsg	loggedInUser[20];
	
	int numBooks = 0;				//count for how many books are in books[20]
	char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
	int sendMsgSize;				/* Size of sent message */
	
	NoneSuchFound.UserID = 0xabcd;
	NoneSuchFound.idok = inValid;
	NoneSuchFound.TCPPort = 0x5555;
	NoneSuchFound.ReqType = Negative;
	
    Ack.UserID = 0xabcd;
	Ack.idok = 1;
	Ack.TCPPort = 0x5555;
	Ack.ReqType = 1;
	
	if(LoggingOn == 1)
	printf(" starting server version 1\n");

	initClientList(loggedInUser);
	
    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }
    
    echoServPort = atoi(argv[1]);  /* First arg:  local port */
    
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
	
	if(LoggingOn == 1)
	printf(" Did not die %d \n", sock);
    
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */
    
    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");
    
	printf(" About to run forever \n");
	
	
	//this is now Login()
	for (;;)  //Run forever 
    {
        // Set the size of the in-out parameter 
        
		if(LoggingOn == 1)
		printf(" About to receieve LoginMsg \n");
		
		recMessage();
       // Block until receive message from a client 
        // if ((recvMsgSize = recvfrom(sock, (char*) &LoginReq, ECHOMAX, 0,
                                    // (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            // DieWithError("recvfrom() failed");
        
		
		//add TCPPort and UserID to the array of Clients
		
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
		printf("TCPPort#: %d UserID: %d \n",LoginReq.TCPPort, LoginReq.UserID);
		
		sendMsgSize = sizeof(Ack);
        /* Send received datagram back to the client */
        /* Send received datagram back to the client */
        if (sendto(sock, (char*) &Ack, sendMsgSize, 0,
                   (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != sendMsgSize)
            DieWithError("sendto() sent a different number of bytes than expected");
			
		
		addClient(LoginReq);
		printClientList(loggedInUser);
    }
	
}	
	
/* 	switch( inputkey ) 
			{
				case 'W':
					//Who call
					TCPID.UserID = 0;  
					TCPID.ReqType = Who;
					sendLogin(servIP,echoServPort,sock,TCPID);
					recMessage();
					
					
					break;
				case 'T':
					//initiate talk session
					TCPID.UserID = 0;  
					TCPID.ReqType = TalkReq;
					
					break;
				case 'L':
					TCPID.UserID = 0;  
					TCPID.ReqType = Logout;
					//logout 
					break;
					
				default :
					//this is invalid input
					break;
			}
			
	}
 */