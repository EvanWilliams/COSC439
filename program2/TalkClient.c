#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/mman.h>  //for mmap.c
#include "Retreive.h"

#define LoggingOn 1
#define ECHOMAX 255     /* Longest string to echo */


void DieWithError(char *errorMessage);  /* External error handling function */

int sendBook(char *servIP,unsigned short echoServPort, int sock, const struct ClientMessage echoStruct);

int sendLogin(char *servIP,unsigned short echoServPort, int sock, const struct loginMsg TCPID);

void recWho();

void printClientList(struct loginMsg loggedInUser[]);
int talkloop(int);
void parentloop();
int childloop(struct loginMsg);
int childAccept();
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
int* childState;					//talkrequest state for child loop
char inputkey;
int UserID01;
int UniquereqID = 0;
int Exitflag = 0;				 /*Exit flag is set to 0 but if the user enters an X it will exit the program*/
int Validflag = 1;				 /*Validation for the operations to make sure it is B/Q/R/X */
	

	
int main(int argc, char *argv[])
{
	int processID;
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
	
	childState = (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
	
    printf("----------------------------------------------------------------\n");

	
	
    if ((argc < 4) || (argc > 5))    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> <Echo Word> [<Echo Port>] <UserID> <TCP Port>\n", argv[0]);
        exit(1);
    }
    
    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
	
    echoServPort = atoi(argv[2]);  /* Use given port, if any */
	
	UserID01 = TCPID.UserID = atoi(argv[3]);               /* unique client identifier */
	TCPID.idok = Valid;  						/* same size as an unsigned int */                             
	TCPID.ReqType = Login;
	TCPID.TCPPort = atoi(argv[4]);	//TCP Port to pass to childStartup
	
	initSock();
	*childState = 0;	//set child loop state 
	
	
	printf("----------------------------------------------------------------\n");
	printf("\n \n Welcome to the talk session. \n");
	printf(" First this client will automatically send your TCP Port and UserID to the server \n");
	printf(" The server will respond with a Success/Failure message \n");
	printf("----------------------------------------------------------------\n");



	if(LoggingOn == 1){
	printf(" about to echo UserID: %d \n",TCPID.UserID);
	printf(" about to echo TCPPort: %d \n",TCPID.TCPPort);
	}
	//logon to server
	sendLogin(servIP,echoServPort,sock,TCPID);
	
	recMessage();
	//Query Case Returns Book details		
		
	printf("\nEnter a key correspoinding to the following choices: \n");
	printf("W---- Who query(retreives all connected client devices)\n");
	printf("T---- Talk Request query(connects to a TCPPort of a connected client device) and \n");
	printf("L---- Logout User (logs the user out) \n");
	
	if ((processID = fork()) < 0)
        DieWithError("fork() failed");
    else if (processID == 0)  /* If this is the child process */
        {
			
            close(sock);   /* Child closes parent socket */
			while(1){
			//	printf("\nCalling Child loop\n");
				childloop(TCPID);
			}
            exit(0);           /* Child process terminates */
        }
		
	parentloop();
	
		Validflag = 1;
	close(sock);
	exit(0);
}

	
void parentloop(){
	int bPrompt = 1;
	int myInt;
	struct loginMsg childaddr;
	while(1){	
		TCPID.UserID = 0;                                
		TCPID.idok = 0;  					                            
		TCPID.TCPPort = 0;
		TCPID.ReqType = 0;
		
		if(bPrompt==1){
			printf("\nEnter command: W for Who, T to initiate Talk session, X to logout");
		}else if(bPrompt==2){
			printf("\nEnter command: A for accept Talk Request,D for Decline\n");
		}
		inputkey = (char) toupper(getchar());
		printf("Childstate : %d \n",*childState);
		if(*childState!=1)
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
						memcpy(&childaddr, &serstructecho, sizeof(struct loginMsg));
						//accept/deny
						*childState = 1;	//set child state to "in a talk session"
						printf("About to start Talk loop Child Addr: %d\n",childaddr.TCPPort);
						talkloop(childaddr.TCPPort);
						//loop
						//client TCP connection
						bPrompt = 2;
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
			else{
				
				switch( inputkey ) 
				{
					
					case 'A':
						//accept/deny talk request
						
						*childState = 1;	//set child state to "in a talk session"
						printf("\nTalk loop initializing on Parent\n");
						talkloop(childaddr.TCPPort);
							
						bPrompt = 1;
						break;
						
					case 'D':
					//accept/deny talk request
					
						*childState = -1;//set child state to "talk rejected"
						printf("Talk Request denied");
					
						bPrompt = 1;
						break;
						
						
					default :
						bPrompt = 0;
						//this is invalid input
						break;
				}
				
				
			}
		}
}




//this is the client child proccess that send the messages to the client Parent
int talkloop(int tcpPort)
{
	//clienttestserver
  int listenfd = 0,connfd = 0;
  
  struct sockaddr_in serv_addr;
 
  char sendBuff[1025];  
  char str1[30];
  int numrv;  
 
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  printf("\nsocket retrieve success\n");
  
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(sendBuff, '0', sizeof(sendBuff));
      
  serv_addr.sin_family = AF_INET;    
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  printf("\nClient Address : %s\n",htonl(INADDR_ANY));
  serv_addr.sin_port = htons(tcpPort);    
  printf("\n TCP Port : %d\n",tcpPort);
 
  bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
   printf("\nBind worked\n");
   sleep(1);
  if(listen(listenfd, 10) == -1){
      printf("Failed to listen\n");
      return -1;
  }
  printf("A talk request has been sent from . Would you like to Accept?");
  printf("Enter A to Accept and D to reject the request.");
  
  childState = 0;
  
  while(*childState == 0)
  {
	sleep(1);
	  
  }
	if(*childState == -1)
		return -1;
  
 connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL); // accept awaiting request
  while(1)
    {
      
	  printf("enter a string less then 30 characters");
	  printf("enter X to exit the talk session");
	  scanf("%s",str1);
	  // if(str1 == 'X')
		  // break;
	  
      strcpy(sendBuff,str1);
      write(connfd, sendBuff, strlen(sendBuff));
    
      sleep(1);
    }
	close(connfd); 
	 
  
 
  return 0;
}



int childloop(struct loginMsg child)
//socettestclient
{
  int sockfd = 0,n = 0;
  char recvBuff[1024];
  struct sockaddr_in serv_addr;
 
  memset(recvBuff, '0' ,sizeof(recvBuff));
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
      printf("\n Error : Could not create socket \n");
      return 1;
    }
	serv_addr.sin_family = AF_INET;
	//printf("%d\n",child.TCPPort);
	serv_addr.sin_port = htons(child.TCPPort);
	//printf("%s\n",inet_ntoa(child.clientIP));
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//printf("\nServer address: %s\n TCP Port :%d ",serv_addr.sin_addr.s_addr,serv_addr.sin_port);
	sleep(1);
  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
    //  printf("\n Error : Connect Failed \n");
	 // printf("Serveraddr: %d,%s",serv_addr.sin_port,serv_addr.sin_addr.s_addr);
      return 1;
    }
	printf("Talk connected\n");
	*childState = 1;
  while((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
      recvBuff[n] = 0;
      if(fputs(recvBuff, stdout) == EOF)
    {
      printf("\n Error : Fputs error");
    }
      printf("\n");
    }
 
  if( n < 0)
    {
      printf("\n Read Error \n");
    }
 

  return 0;
}


	
void printClientList(struct loginMsg loggedInUser[])
{
	//prints out the client list if the UserID = Zero then break
	printf("Printing Client List");	
	int i;
	for (i = 0; i < 20; i++){
		
	if( loggedInUser[i].UserID == 0)
		break;
	printf("\nClient #:%d Has a UserId:%d and a TCPPort:%d as well as a idok:%d \n IP Address : %s ",i,loggedInUser[i].UserID, loggedInUser[i].TCPPort, loggedInUser[i].idok,inet_ntoa(loggedInUser[i].clientIP));
	
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
				printf("\nUserID: %d \nHas a TCP Port # of %d\n",serstructecho.UserID,serstructecho.TCPPort);
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


	
	




