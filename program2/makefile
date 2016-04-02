all:	talkclient.exe

talkclient.exe:	talkclient.o DieWithError.o
	gcc -o talkclient.exe talkclient.o DieWithError.o
		
talkclient.o:	TalkClient.c 
	gcc -g -o talkclient.o -c TalkClient.c 
	
DieWithError.o:	DieWithError.c 
	gcc -c DieWithError.c 