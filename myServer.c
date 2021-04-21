/******************************************************************************
* tcp_server.c
*
* CPE 464 - Program 1
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "networks.h"
#include "message.h"
#include "pollLib.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

int process_event(int clientSocket);
void recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    int socket;
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;

	portNumber = checkArgs(argc, argv);

	//create the server socket
	serverSocket = tcpServerSetup(portNumber);

    // add server socket to the pollset
    addToPollSet(serverSocket);

    // wait for client to connect
    while(1){

        if((socket = pollCall(-1)) == serverSocket){
            clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
            addToPollSet(clientSocket);
        }
        else{
            process_event(socket);
        }
    }
	close(serverSocket);
	return 0;
}

int process_event(int clientSocket)
{
    int received = MAXBUF; // just needs to be > 0
    char receiveBuf[MAXBUF];
    char sendBuf[MAXBUF];

    received = msg_receive(clientSocket, receiveBuf);

    if(received == 0 || !strcmp(receiveBuf, "exit")){
        printf("client disconnected\n");
        removeFromPollSet(clientSocket);
        close(clientSocket);
        return 0 ;
    }

    sprintf(sendBuf, "recv() Len %d, PDU Len: %d, Message: %s",
            received, received, receiveBuf);
    msg_send(clientSocket, sendBuf, strlen(sendBuf)+1);

    memset(sendBuf, 0, MAXBUF);
    sprintf(sendBuf, "Number of bytes received by the server was: %d",
            received);
    msg_send(clientSocket, sendBuf, strlen(sendBuf)+1);


    return 0;


}

void recvFromClient(int clientSocket)
{
	char buf[MAXBUF];
	int messageLen = 0;

	//now get the data from the client_socket
	if ((messageLen = recv(clientSocket, buf, MAXBUF, 0)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	printf("Message received, length: %d Data: %s\n", messageLen, buf);
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}

	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}

	return portNumber;
}

