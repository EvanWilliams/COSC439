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

void recWho();

void printClientList(struct loginMsg loggedInUser[]);

void parentloop();

void recMessage();
void initSock();

int sock;                        /* Socket descriptor */
struct sockaddr_in echoServAddr; /* Echo server address */
struct sockaddr_in fromAddr;     /* Source address of echo */
unsigned short echoServPort;     /* Echo server port */
unsigned int fromSize;           /* In-out of address size for recvfrom() */
char *servIP;                    /* IP address of server */
struct ClientMessage echoStruct; /* String to send to echo server */		
struct loginMsg TCPID;			 /* Struct to send the Login Credentials to the server*/
struct loginMsg	loggedInUser[20];// This is the array to stor clients from who queries
struct loginMsg serstructecho;	 /*Recieved struct from server side*/
char *requestString; 			 /* Enum String for output echoStruct.requesttype*/
char *respTypeString;
char *ClientRqtype;				 /* Request type specified by the user Q/B/R */
char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
int echoStructLen;               /* Length of string to echo */
int respStringLen;               /* Length of received response */
int recvMsgSize;
char inputkey;
int UserID01 = 3279;
int UniquereqID = 0;
int Exitflag = 0;				 /*Exit flag is set to 0 but if the user enters an X it will exit the program*/
int Validflag = 1;				 /*Validation for the operations to make sure it is B/Q/R/X */
	

int main(int argc, char *argv[])
{
	extern int sock;                        /* Socket descriptor */
    extern struct sockaddr_in echoServAddr; /* Echo server address */
    extern unsigned short echoServPort;     /* Echo server port */
	extern struct loginMsg serstructecho;	 /*Recieved struct from server side*/
    extern char *servIP;                    /* IP address of server */
	extern struct loginMsg TCPID;			 /* Struct to send the Login Credentials to the server*/
    extern int respStringLen;               /* Length of received response */
	extern int recvMsgSize;
	extern char inputkey;
	extern int UserID;
	extern int UniquereqID;
	extern int Exitflag;				 /*Exit flag is set to 0 but if the user enters an X it will exit the program*/
	extern int Validflag;				 /*Validation for the operations to make sure it is B/Q/R/X */
	
    printf("----------------------------------------------------------------\n");
	TCPID.UserID = 3279;                                	 /* unique client identifier */
	TCPID.idok = Valid;  						/* same size as an unsigned int */                             
	TCPID.TCPPort = 2727;
	TCPID.ReqType = Login;
	
	
    if ((argc < 2) || (argc > 3))    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
        exit(1);
    }
    
    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]);  /* Use given port, if any */
	
	initSock();
		
	
	printf("----------------------------------------------------------------\n");
	printf("\n \n Welcome to the talk session. \n");
	printf(" First this client will automatically send your TCP Port and UserID to the server \n");
	printf(" The server will respond with a Success/Failure message \n");
	printf("----------------------------------------------------------------\n");



	if(LoggingOn == 1){
	printf(" about to echo UserID: %d \n",TCPID.UserID);
	printf(" about to echo TCPPort: %d \n",TCPID.TCPPort);
	}
	
	sendLogin(servIP,echoServPort,sock,TCPID);
	
	recMessage();
	//Query Case Returns Book details		
		
	printf("\nEnter a key correspoinding to the following choices: \n");
	printf("W---- Who query(retreives all connected client devices)\n");
	printf("T---- Talk Request query(connects to a TCPPort of a connected client device) and \n");
	printf("L---- Logout User (logs the user out) \n");
	
	parentloop();
	
		Validflag = 1;
	close(sock);
	exit(0);
}
	
void parentloop(){
	int bPrompt = 1;
	int myInt;
	while(1){	
	TCPID.UserID = 0;                                
	TCPID.idok = 0;  					                            
	TCPID.TCPPort = 0;
	TCPID.ReqType = 0;
	
	if(bPrompt==1){
	printf("\nEnter command: W for Who, T to initiate Talk session, X to logout");
	}
	inputkey = (char) toupper(getchar());
	
		switch( inputkey ) 
			{
				case 'W':
					//Who call
					TCPID.UserID = UserID01;  
					TCPID.ReqType = Who;
					sendLogin(servIP,echoServPort,sock,TCPID);
					recWho(loggedInUser);
					printClientList(loggedInUser);
					bPrompt = 1;
					break;
					
				case 'T':
					//initiate talk session
					printf("\nEnter a UserID to Start a talk Session with \n"); 
					scanf("%d", &myInt);
					TCPID.UserID = myInt;
					TCPID.ReqType = TalkReq;
					//Address Lookup
					sendLogin(servIP,echoServPort,sock,TCPID);
					//recieve the login with the corresponding User's TCPPort
					recMessage();
					//accept/deny
					//client TCP connection
					bPrompt = 1;
					break;
					
				case 'X':
					TCPID.UserID = UserID01;  
					TCPID.ReqType = Logout;
					sendLogin(servIP,echoServPort,sock,TCPID);
					printf("Logging Out...");
					exit(0);
					//logout 
					break;
					
				default :
					bPrompt = 0;
					//this is invalid input
					break;
			}
			
	}
}
	
void printClientList(struct loginMsg loggedInUser[])
{
	//prints out the client list if the UserID = Zero then break
	printf("Printing Client List");	
	int i;
	for (i = 0; i < 20; i++){
		
	if( loggedInUser[i].UserID == 0)
		break;
	printf("Client #:%d Has a UserId:%d and a TCPPort:%d as well as a idok:%d \n ",i,loggedInUser[i].UserID, loggedInUser[i].TCPPort, loggedInUser[i].idok);
	
	}
}	
	
	
void initSock(){
	extern int sock;    
			/* Create a datagram/UDP socket */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");
		
	
}
void recWho(){
	extern int sock;                        /* Socket descriptor */
	extern int respStringLen;               /* Length of received response */
	extern struct loginMsg	loggedInUser[20];
	extern struct sockaddr_in fromAddr;     /* Source address of echo */
	extern unsigned int fromSize;           /* In-out of address size for recvfrom() */
	
	respStringLen = sizeof(struct loginMsg)*20;
	
	 //Recv a response 
	fromSize = sizeof(fromAddr);
	if ((recvfrom(sock, (char*) &loggedInUser, respStringLen, 0,
		 (struct sockaddr *) &fromAddr, &fromSize)) != respStringLen)
		DieWithError("recvfrom() failed");

		printf(" Test %d \n",echoStruct.requestType+1);
	
}	
	
 void recMessage(){
	 extern int sock;                        /* Socket descriptor *///
	 extern struct loginMsg serstructecho;	 /*Recieved struct from server side*/
	 extern int respStringLen;               /* Length of received response */
	 extern struct sockaddr_in fromAddr;     /* Source address of echo */
	 extern unsigned int fromSize;           /* In-out of address size for recvfrom() */
	
			respStringLen = sizeof(serstructecho);
	
	 //Recv a response 
		fromSize = sizeof(fromAddr);
			if ((respStringLen = recvfrom(sock, (char*) &serstructecho, respStringLen, 0,
				 (struct sockaddr *) &fromAddr, &fromSize)) != respStringLen)
				DieWithError("recvfrom() failed");
			if(serstructecho.UserID == 0x777){
				
				printf("\nNo User With That ID was found");
			}else{
				printf("\nUserID: %d Has a TCP Port # of %d",serstructecho.UserID,serstructecho.TCPPort);
			}
	if(serstructecho.idok == Valid)
		printf(" \n Server responds-- Address and TCP Port number is valid");
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


	
	




