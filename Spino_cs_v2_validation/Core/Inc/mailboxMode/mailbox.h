#ifndef MAILBOX_H
#define MAILBOX_H

#include "../ax25/ax25.h"

#define MAX_MAILBOX 32
#define MAX_MESSAGE 16

#define MAX_LENGHT_MESSAGE 220
#define CALLSIGN_SIZE 6

/* COMMANDE MAILBOX */

#define CMD_MAILBOX_INIT 32
#define CMD_MAILBOX_ADD_MSG 33 
#define CMD_MAILBOX_DEL_MSG 34 
#define CMD_MAILBOX_GET_LIST_BOX 35
#define CMD_MAILBOX_DELETTE_BOX 36
#define CMD_MAILBOX_GET_LAST_MSG 37
#define CMD_MAILBOX_GET_MSG 38
#define CMD_MAILBOX_GET_ALL_MSG 39
#define CMD_MAILBOX_DUMP_MAILBOX 40 

/**
 * \struct t_mailbox_message
 * \brief define mailbox structure to store message for a dedicated Callsign 
 *
 */

typedef struct mailbox_message {
	unsigned long long timestamps; /*!< Time Stamps of message creation */
	unsigned short size; /*!< message size. Should <=  MAX_LENGHT_MESSAGE*/
	char message[MAX_LENGHT_MESSAGE]; /*!< Message contents  */
} s_mailbox_message;

/**
 * \struct t_mailbox_message
 * \brief define mailbox structure to store message for a dedicated Callsign 
 *
 */

typedef struct mail_box {
	char messageNumber;
	unsigned char indexNextMessage;
	unsigned char indexFreeMessage;
	unsigned char callsign[CALLSIGN_SIZE];
	unsigned long long timestampCreation;
	s_mailbox_message messages[MAX_MESSAGE];
	/* data */
} s_mailbox;

typedef struct mailboxes {

	s_mailbox mailbox[MAX_MAILBOX];
	int usedMailboxNumber;
	int indexfreeMailbox;
//	long nbMailboxCommandeReceived;
//	long nbMailboxErrorCommandeReceived;
} s_mailboxes;

typedef struct listMailbox {
	char callsign[6];
	char nb_message;
} s_list_mailbox;

typedef struct add_message {
	char size;
	char message[MAX_LENGHT_MESSAGE];

} s_add_message;

#define SIZE_T_GET_MESSAGE 15
typedef struct get_message {
	unsigned long long timestamps;
	char callsign[6];
	unsigned char index;
	char message[MAX_LENGHT_MESSAGE];

} s_get_message;

typedef struct cmd_get_message {
	unsigned char index;
	char callsign[6];

} s_cmd_get_message;

void processMailbox(s_ax25_packet data_ax25);
unsigned char getListMailbox(t_tc_response *resp);
unsigned char addMessage(unsigned char *callsign, char *message,
		const unsigned short sizemMessage);
unsigned char deleteMailBox(unsigned char *callsign);
unsigned char initialise();
unsigned char getAllMesage(unsigned char *callsign);

#endif // MAILBOX_H
