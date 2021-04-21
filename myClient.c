/******************************************************************************
* myClient.c
*
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

//#define MAXBUF 1024
#define DEBUG_FLAG 1

int client_loop(int socketNum);
void sendToServer(int socketNum);
int readFromStdin(char * buffer);
void checkArgs(int argc, char * argv[]);

/*
   Prompt for the user input
   Send PDU with user input
   Receive first server response and print
   Receive second server response and print
   Go back to prompt user input
*/

int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor

	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[1], argv[2], DEBUG_FLAG);

    client_loop(socketNum);
	//sendToServer(socketNum);

	close(socketNum);

	return 0;
}

int client_loop(int socketNum)
{
	char sendBuf[MAXBUF];   //data buffer
    uint16_t sendBuf_size;
	char receiveBuf[MAXBUF];   //data buffer

    int bytesReceived_1 = MAXBUF; /* Arbitrary */
    int bytesReceived_2 = MAXBUF; /* Arbitrary */

    while(bytesReceived_1 >= 0 || bytesReceived_2 >= 0)
    {
        sendBuf_size = readFromStdin(sendBuf);
        msg_send(socketNum, sendBuf, sendBuf_size);

        if(!strcmp(sendBuf, "exit"))
            break;

        bytesReceived_1 = msg_receive(socketNum, receiveBuf);
        // "recv() Len 8, PDU Len: 8, Message: hello"
        printf("Recv() from Server: %s\n", receiveBuf);

        bytesReceived_2 = msg_receive(socketNum, receiveBuf);
        // "Number of bytes received by the server was: XXX"
        printf("Recv() from Server: %s\n", receiveBuf);
    }

    return 0;

}

void sendToServer(int socketNum)
{
	char sendBuf[MAXBUF];   //data buffer
	int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */

	sendLen = readFromStdin(sendBuf);
	printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);

	sent =  send(socketNum, sendBuf, sendLen, 0);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}

	printf("Amount of data sent is: %d\n", sent);
}

int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;

	// Important you don't input more characters than you have space
	buffer[0] = '\0';
	printf("Enter data: ");
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}

	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;

	return inputLen;
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 3)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}
