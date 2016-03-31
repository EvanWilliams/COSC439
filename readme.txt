Evan Williams for Class COSC439 by Elsa Poh

to compile this program on Emunix--

My log in credintials are :
					username=ew4390

1. cd into the correct directory:  cd C\ Files/
2. compile server side: gcc -o server.exe RetreivebookServer.c DieWithError.c
3. run the server program: ./server.exe 27000 
4. Open another putty window
5. compile client side: gcc -o client.exe RetreivebookClient.c DieWithError.c
6. Run the client program : ./client.exe 127.0.0.1 27000

Next the program will ask you to enter (B)or (Q) or (R) or (X).

After you make your choice you will be asked to enter a valid ISBN
--if your ISBN is invalid it will ask you once again for a valid ISBN

This process will continue until you enter a (X) to exit the program.

This has been tested on Emunix as well as from a PC client to a linux server (http://www.sshvm.com/)

