#ifndef PAYLOG_MODE_H
#define PAYLOG_MODE_H

#define I2CMAXSIZE 256
#define I2CMAXDATA 5

#define CMD_PAYLOAD_LOAD_DATA 10
#define CMD_PAYLOAD_READ_DATA 11
#define CMD_PAYLOAD_READ_DATA_ALL 12
typedef struct I2C_data {
	unsigned char size;
	char data[I2CMAXSIZE];

} s_I2C_data;

#endif // PAYLOG_MODE_H
