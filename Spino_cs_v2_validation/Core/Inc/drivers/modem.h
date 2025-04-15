#ifndef MODEM_H
#define MODEM_H

#include "../ax25/ax25.h"

#define SIMU_MODE_FILE 1
#define SIMU_MODE_TCP 2

#define MAX_MODE 4
#define DELAY_MULTIMODE 100

int setModemMode(uint8_t mode);
void ModemOpen();
int readData(unsigned char *data);
int readDataDebug(unsigned char *data);
int readDataRX2(unsigned char *data);
int readDataRX(uint8_t** access);
int writeDataDebug(const s_ax25_packet ax25Frame, const int length);
int writeDataTX(const s_ax25_packet ax25Frame, const int length);
int writeData(s_ax25_packet ax25Frame, int length);

extern int gv_simu_mode;

#endif // MODEM_H
