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

int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    struct ServerMessage serstructecho;    /* Buffer for echo string */
	struct ServerMessage books[20]; //array of books that are read from the text file
	struct ClientMessage echoStruct; //clientmessage struct
	
	struct loginMsg	LoginReq;
	struct loginMsg	loggedInUser[20];
	
	int numBooks = 0;				//count for how many books are in books[20]
	char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
	int sendMsgSize;				/* Size of sent message */
    
	if(LoggingOn == 1)
	printf(" starting server version 1\n");
	
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
        cliAddrLen = sizeof(echoClntAddr);
        
		if(LoggingOn == 1)
		printf(" About to receieve LoginMsg \n");
		
        //Block until receive message from a client 
        if ((recvMsgSize = recvfrom(sock, (char*) &LoginReq, ECHOMAX, 0,
                                    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");
        
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
		printf("TCPPort#: %d UserID: %d \n",LoginReq.TCPPort, LoginReq.UserID);
		
		
    }
	
}	
	


int findbooks(struct ServerMessage *books,char isbn[13],int numBooks)
{
	int i;
	//returns an index of the book if found if it is not found return -1
	for(i = 0; i<numBooks; i++)
	{
		if (memcmp(isbn, books[i].isbn, 13)== 0)
		{
			return (i);
		}
		
	}
	
return (-1);
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

		if(LoggingOn == 1)//debug logging
		printf("isbn checkdigit : %d\n",checkDigit);
	
        return result;
    }

int loadbooks(struct ServerMessage *books)
{
	
	FILE * fp;
	char isbn[13];                                          /* book ISBN-13*/
	char authors[100];										/* book author(s) */
	char title[100];										/* book title */
	unsigned int edition;									/* book edition */
	unsigned int year;										/* book publication year */
	char publisher[100]; 									/* book author(s) */
	unsigned int inventory;									/* inventory count */
	unsigned int available;									/* number of available books */
	int numBooks = 0;
	
   
   fp = fopen ("books.txt", "r");
   
   if(LoggingOn == 1)//debug logging
   printf("eof status: %d \n",feof(fp));
   
   while(feof(fp) ==0){//Scans in the book entries one line at a time and returns the entries into the "books" struct array
   fscanf(fp, "%13c %s %s %d %d %s %d %d \n", books[numBooks].isbn, books[numBooks].authors, books[numBooks].title, &books[numBooks].edition,&books[numBooks].year,books[numBooks].publisher,&books[numBooks].inventory,&books[numBooks].available);
   
   if(LoggingOn == 1)//debug logging
   printf("%s %s %s %d %d %s %d %d\n", books[numBooks].isbn, books[numBooks].authors, books[numBooks].title, books[numBooks].edition,books[numBooks].year,books[numBooks].publisher,books[numBooks].inventory,books[numBooks].available);

   numBooks++;//to keep count of how many books there are
   }
   return (numBooks);
}
