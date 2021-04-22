
#include "message.h"

#define CONNECTION_CLOSED 0

int msg_receive(int socket_fd, char *buf)
{
    int messageLen;
    int bytesReceived;
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


    if((bytesReceived = recv(socket_fd, buf, ntohs(msg_size) - HDR_LEN, MSG_WAITALL)) < 0)
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
	int sent = 0;            /* actual amount of data sent get the data and send it   */
    int host_pdu_length;

    struct PDU {
        uint16_t length;
        char buf[sendLen];
    } __attribute__((packed)) pdu;

    host_pdu_length = HDR_LEN + sendLen;
    pdu.length = htons(host_pdu_length);

    strcpy(pdu.buf, sendBuf);

	//printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);

	sent =  send(socket_fd, (char *) &pdu, host_pdu_length, 0);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}


	//printf("Amount of data sent is: %d\n", sent);
    return sent;
}
