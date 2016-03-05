#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

enum rectype{Query, Borrow, Return};
struct ClientMessage{
unsigned int requestID;                             /* unique client identifier */
enum rectype requestType;         /* same size as an unsigned int */
char isbn[13];                                       /* book’s validated ISBN-1 */
}; 