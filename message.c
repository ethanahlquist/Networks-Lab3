//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/uio.h>
//#include <sys/time.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <string.h>
//#include <strings.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>

#include "message.h"

#define CONNECTION_CLOSED 0

int msg_receive(int socket_fd, char *buf)
{
    /* */
    //int bufSize = MAXBUF;
    //static const char zero[bufSize] = { 0 };
    //const static char zero[MAXBUF] = { 0 };

    /* Recieve a */

    int messageLen;
    int bytesReceived;

    //uint16_t PDU_len; // this might need a ntohs
    uint16_t msg_size;


    if((messageLen = recv(socket_fd, (char *)&msg_size, HDR_LEN, MSG_WAITALL)) < 0)
    {
		perror("recv call");
		exit(-1);
    }
    else if (messageLen == 0)
    {
		//perror("connection termination");
        return 0;
    }


    if((bytesReceived = recv(socket_fd, buf, msg_size - HDR_LEN, MSG_WAITALL)) < 0)
    {
		perror("recv call");
		exit(-1);
    }
    else if (bytesReceived == 0)
    {
		//perror("connection termination");
        return 0;
    }
	//printf("Message received, length: %d Data: %s\n", messageLen, buf);

    return bytesReceived;
}

int msg_send(int socket_fd, char *sendBuf, uint16_t sendLen)
{

	//char sendBuf[MAXBUF];   //data buffer
	//int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */

    struct PDU {
        uint16_t length;
        char buf[sendLen];
    } __attribute__((packed)) pdu;

    //memset(&pdu, 0, sizeof(pdu));
    pdu.length = HDR_LEN + sendLen;
    strcpy(pdu.buf, sendBuf);

	//printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);

	sent =  send(socket_fd, (char *) &pdu, pdu.length, 0);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}

	//printf("Amount of data sent is: %d\n", sent);
    return sent;
}
