#ifndef COMMAND_H
#define COMMAND_H

#define PARAMETER_SIZE 256   /*!< Max size of command parameter. */
#define FIELD_SIZE 32       /*!< Max size of field parameter. */

/* define type of response */

#define RESULT_CMD 128     /*!< response type :  Result from command received by SPINO */
#define TELEMETRY 64       /*!< response type :  SPINO Telemetry */
#define INFORMATION_MSG 65 /*!< response type :  SPINO Information Message */
#define RESULT_DROP_MESSAGE 32     /*!< response type :  Drop message */
#define EXPEBEACON  16     /*!< response type :  Drop message */

/* COMMAND LIST */
#define CMD_RESET 100  
#define SET_VALUE 101
#define GET_VALUE 102
#define GET_CONGIG 103
#define GET_LAST_DROPED_MESSAGE 120
#define GET_ALL_DROPED_MESSAGE 121
#define GET_DROP 122
#define GET_LAST_LOG 130
#define GET_ALL_LOG 131
#define GET_LOG 134
#define SET_INFO_MESSAGE 132
#define DEL_INFO_MESSAGE 133

#define SET_STATE 255

/* load new program */
#define PROG_INIT 64
#define PROG_LOAD 65
#define PROG_CHECK 66 
#define PROG_SET_ADDRESS 67

/* SET GET VALUE */

#define VALUE_SPINO_VERSION 128
#define VALUE_SPINO_DELAY 1
#define VALUE_CALLSIGN_SRC_SPINO 2 
#define VALUE_CALLSIGN_DES_SPINO 3
#define VALUE_CALLSIGN_PAYLOAD_SPINO 4
#define VALUE_TIMESTAMP 5
#define VALUE_LOG_LEVEL 6
#define VALUE_ACTIVE_INFO_MESSAGE 7
#define VALUE_DELAY_INFO_MESSAGE 8
#define VALUE_MODEM_TX_MODE 9
#define VALUE_MODEM_TX_MULTIMODE 10

#define PROG_INDEX 4

typedef struct tm_tc_header {
	unsigned short size;
	unsigned short cmd_id;
	unsigned char responseType;
	unsigned char error_code;
	unsigned short spare;
	unsigned long long timeStamp;

} s_tm_tc_header;

#define TC_REPONSE_HEADER_SIZE  sizeof(s_tm_tc_header) + 2 // taille de la partie fixe + 2 pour taille de la size

typedef struct tc_response {

	s_tm_tc_header header;
	unsigned short size;
	char parameter[PARAMETER_SIZE];
} t_tc_response;

#define SIZE_HEADER_FIELD 2

typedef struct field {
	unsigned char field_id;
	unsigned char size;
	unsigned char field_value[FIELD_SIZE];


} s_field;

typedef struct command {
	unsigned short size; /* parameter size */
	unsigned short key; /* clé */
	unsigned short id; /* command ID */
	char parameter[PARAMETER_SIZE];
} s_command;

typedef struct set_value {
	unsigned char fied_id;
	unsigned char size;
	char value[FIELD_SIZE];

} s_set_value;

typedef struct get_value {
	unsigned char field_id;
	unsigned char size;

} s_get_value;

#define MAX_MEM_PRG 4096

typedef struct prog_mngt {
	long indexCourrant;
	char *memory;
} s_prog_mngt;

#define MAX_MEM_PRG_LOAD  64

typedef struct load_prog {
	long index;
	char mem1[MAX_MEM_PRG_LOAD];
	char mem2[MAX_MEM_PRG_LOAD];
} s_load_prg;

t_tc_response interpretcommand(s_command cmd);
void processCommand(s_ax25_packet data_ax25);
void processDropMessage(char *data_ax25, unsigned short size);

#endif // COMMAND_H
