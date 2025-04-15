#ifndef INFORMATION_MESSAGE_H
#define INFORMATION_MESSAGE_H

#include "command.h"

#define MAX_SIZE_INF_MSG 256
#define MAX_INF_MESSAGE 10

#define INFO_MSG_NOT_USED 0
#define INFO_MSG_USED 1

typedef struct inf_msg {
	char message[MAX_SIZE_INF_MSG];
	unsigned char used;

} s_inf_msg;

typedef struct add_inf_msg {
	unsigned char index;
	char message[MAX_SIZE_INF_MSG];
} s_add_inf_msg;

typedef struct send_in_msg {
	unsigned char index;
	unsigned char used;
	char message[MAX_SIZE_INF_MSG];
} s_send_inf_msg;

unsigned char setInfoMessage(char *data, t_tc_response *resp);
unsigned char delInfoMessage(char index, t_tc_response *resp);
void sendInfoMessage();
void setupInfoMessage();

#endif // INFORMATION_MESSAGE_H
