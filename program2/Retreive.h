#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <ctype.h>

//struct sendBook(sockaddr_in echoServAddr,char servIP,unsigned short echoServPort, int sock, ClientMessage echoString){}

 struct ServerMessage{
	unsigned int requestID;                                 /* unique client identifier */
	enum {Okay, ISBNError, AllGone, NoInventory} respType;  /* same size as an unsigned int */
	char isbn[13];                                          /* book ISBN-13*/
	char authors[100];										/* book author(s) */
	char title[100];										/* book title */
	unsigned int edition;									/* book edition */
	unsigned int year;										/* book publication year */
	char publisher[100]; 									/* book author(s) */
	unsigned int inventory;									/* inventory count */
	unsigned int available;									/* number of available books */
};

enum rectype{Query, Borrow, Return};
struct ClientMessage{
unsigned int requestID;                             /* unique client identifier */
enum rectype requestType;         /* same size as an unsigned int */
char isbn[13];                                       /* book’s validated ISBN-1 */
}; 

struct loginMsg{
	unsigned int UserID;                                	 /* unique client identifier */
	enum {Valid, inValid} idok;  						/* same size as an unsigned int */    
	enum {Login, Who, TalkReq, Logout, Negative} ReqType;	
	unsigned int TCPPort;									/* book edition */
	
};