#ifndef EXPERIMENTAL_MODE_H
#define EXPERIMENTAL_MODE_H

#define SIZE_TLE 70
#define CMD_LOAD_TLE_1 70
#define CMD_LOAD_TLE_2 71
#define CMD_DOWNLOAD_TLE 72
#define CMD_DOWNLOAD_TLE_1 73
#define CMD_DOWNLOAD_TLE_2 74
#define CMD_EXP_ADD_DATA 75
#define CMD_EXPE_GET_LIST 76
#define CMD_EXPE_DELETTE_ALL 77
#define CMD_EXPE_GET_ALL_DATA 78
#define CMD_EXPE_INIT 79
#define CMD_SET_EXPE_BEACON_DELAY 80
#define CMD_BEACON  81

typedef struct tle {
	char tleLine1[SIZE_TLE];
	char tleLine2[SIZE_TLE];

} s_tle;

typedef struct experimental_beacon {

	long id;
	long  expereceived;
	long  expereceivedfail;
	short delay;
	short idf;

} s_experimental_beacon;

void initExpe();
void sendBeaconExpe();
unsigned short experimentalMode();

#endif //EXPERIMENTAL_MODE_H
