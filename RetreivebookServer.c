#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "Retreive.h"

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
	struct ServerMessage books[20];
	
	int numBooks = 0;
	char echoBuffer[ECHOMAX];        /* Buffer for echo string */
	struct ClientMessage echoStruct;
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
	int sendMsgSize;
    
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
	numBooks = loadbooks(books);
	
	
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);
        
		printf(" About to receieve mesg \n");
		
        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, (char*) &echoStruct, ECHOMAX, 0,
                                    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");
        
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
		/*This checks if the book corresponding to the ISBN is in the database*/
		if(findbooks(books,echoStruct.isbn,numBooks)!=-1){
			serstructecho = books[findbooks(books,echoStruct.isbn,numBooks)];
			/*if the book is available then set the respType to 0 meaning "Okay"*/
			serstructecho.respType = 0;
			
		if(echoStruct.requestType == 1)
		{
			if(serstructecho.available>0)
			{
				serstructecho.available--;
				serstructecho.respType = 0;
			}
			else{
				serstructecho.respType = 2;
			}
		}
		else if(echoStruct.requestType == 2)
		{
			serstructecho.available++;
			serstructecho.respType = 0;
		}
		
		}
		else{
			serstructecho.respType = 1;
		}
		printf("%s\n",serstructecho.authors);
		sendMsgSize = sizeof(serstructecho);
		
        /* Send received datagram back to the client */
        /* Send received datagram back to the client */
        if (sendto(sock, (char*) &serstructecho, sendMsgSize, 0,
                   (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != sendMsgSize)
            DieWithError("sendto() sent a different number of bytes than expected");
		
		printf(" Sending string %s \n",serstructecho.authors);
    }
    /* NOT REACHED */
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
   printf("eof status: %d \n",feof(fp));
   
   while(feof(fp) ==0){
   fscanf(fp, "%13c %s %s %d %d %s %d %d \n", books[numBooks].isbn, books[numBooks].authors, books[numBooks].title, &books[numBooks].edition,&books[numBooks].year,books[numBooks].publisher,&books[numBooks].inventory,&books[numBooks].available);
   printf("%s %s %s %d %d %s %d %d\n", books[numBooks].isbn, books[numBooks].authors, books[numBooks].title, books[numBooks].edition,books[numBooks].year,books[numBooks].publisher,books[numBooks].inventory,books[numBooks].available);
   numBooks++;
   }
   return (numBooks);
}
