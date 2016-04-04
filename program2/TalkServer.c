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
 struct loginMsg LoginReq;
 struct sockaddr_in echoServAddr; /* Local address */
 struct sockaddr_in echoClntAddr; /* Client address */
 int recvMsgSize;
 unsigned int cliAddrLen; 
 

void initClientList(struct loginMsg loggedInUser[])
{
	int i;
	for (i = 0; i < 20; i++){
		loggedInUser[i].UserID = 0;
	}
}	
	
int findLastEntry(struct loginMsg loggedInUser[])
{
	int i;
	for (i = 0; i < 20; i++){
		if( loggedInUser[i].UserID == 0){
			return(i);
		}
		
	}
	return(-1);
}



void removeClient(unsigned int remvUserID){
	
	extern struct loginMsg loggedInUser[20];
	
	int remvClient = findUserID(loggedInUser,remvUserID);
	if(remvClient !=0x777 ){
		loggedInUser[remvClient].UserID = 0;
	}
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
	printf("Looking for User");	
	int i;
	for (i = 0; i < 20; i++){
		
	if( loggedInUser[i].UserID == findUserID)
		return i;
	
	}
	return 0x777;
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

int sendClient(int sock, struct loginMsg Lmesg, struct sockaddr_in echoServAddr)
{
	size_t loginMsgLen = sizeof(Lmesg);
	    /* Construct the server address structure */
		
	if(LoggingOn == 1)
	printf(" Checking echo string %i \n",Lmesg.TCPPort);
    
    /* Send the Struct to the server */
	int testnum = (sendto(sock, (char*) &Lmesg, loginMsgLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)));
	
    if (testnum != loginMsgLen)
        DieWithError("sendto() sent a different number of bytes than expected");
		
	printf(" Sending Message: to Client %d \n",Lmesg.ReqType);
}

int sendWho(int sock, struct loginMsg *loggedInUser, struct sockaddr_in echoServAddr)
{
	size_t loginMsgLen = sizeof(struct loginMsg)*20;
    
    /* Send the string to the server */
    if (sendto(sock, (char*) loggedInUser, loginMsgLen, 0, (struct sockaddr *)
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
		printf(" About to receive LoginMsg \n");
		
        //Block until receive message from a client 
        if ((recvMsgSize = recvfrom(sock, (char*) &LoginReq, ECHOMAX, 0,
                                    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");
        
		printf(" Length of Mesg Received:%d %s \n",recvMsgSize,inet_ntoa(echoClntAddr.sin_addr));
		LoginReq.clientIP = echoClntAddr.sin_addr;
		
}	

int main(int argc, char *argv[])
{
    extern int sock;                        /* Socket */
    extern struct sockaddr_in echoServAddr; /* Local address */
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
	int UserLoc;//this is the index that is used by findUserID to send the appropriate Client info 

	NoneSuchFound.UserID = 0x777;
	NoneSuchFound.idok = inValid;
	NoneSuchFound.TCPPort = 0x777;
	NoneSuchFound.ReqType = Negative;
	
	LoginReq.UserID = 123;
	LoginReq.idok = 1;
	LoginReq.TCPPort = 1555;
	LoginReq.ReqType = Login;
	
    Ack.UserID = 0xabcd;
	Ack.idok = 1;
	Ack.TCPPort = 0x5555;
	Ack.ReqType = Login;
	
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
		
		memset(&LoginReq, 0, sizeof(LoginReq));
		
		recMessage();
		
			switch( LoginReq.ReqType) 
			{
				case Login:
					//login call
					addClient(LoginReq);
					
					Ack.ReqType = Login;
					
					sendClient(sock,Ack,echoClntAddr);
					printf(" About to receive LoginMsg \n");
					break;
					
				case Who:
					//Who call
					sendWho(sock,loggedInUser,echoClntAddr);
					break;
					
				case TalkReq:
					//initiate talk session
					UserLoc = findUserID(loggedInUser,LoginReq.UserID);
					if(UserLoc == 0x777){
						printf("None Such Found");
						sendClient(sock,NoneSuchFound,echoClntAddr);
					}else{
						//address lookup
					sendClient(sock,loggedInUser[UserLoc],echoClntAddr);
					}
					break;
					
				case Logout:
					removeClient(LoginReq.UserID);
					printf("\nClient with UserId: %d was removed from the list of logged in users\n", LoginReq.UserID);
					//logout 
					break;
					
				default :
					//this is invalid input
					break;
			}
			
	}
		
		
		//add TCPPort and UserID to the array of Clients
		
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
		printf("TCPPort#: %d UserID: %d \n",LoginReq.TCPPort, LoginReq.UserID);
    }
	