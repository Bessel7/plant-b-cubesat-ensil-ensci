#ifndef DOPMESSAGE_H
#define DOPMESSAGE_H

#define MAX_UNPROCESSED_MESSAGE_LENGHT 230
#define MAX_UNPROCESSED_MESSAGE_LIST_SIZE 5

#define SIZE_HEADER_DROP 8+2  /*taille de la structure hors data */
typedef struct unprocessedmessage {
	unsigned long long timestamps;
	unsigned short size;
	char data[MAX_UNPROCESSED_MESSAGE_LENGHT];
} s_unprocessedmessage;

typedef struct unprocessedmessageList {
	int index;
	s_unprocessedmessage message[MAX_UNPROCESSED_MESSAGE_LIST_SIZE];

} s_unprocessedmessageList;

void processDropMessage(char *data_ax25, unsigned short size);
unsigned char getLastDroppedMessage(t_tc_response *resp);
unsigned char getAllDroppedMessage(t_tc_response *resp);

#endif // DOPMESSAGE_H
