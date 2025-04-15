#ifndef LOG_H
#define LOG_H

#define LOG_LEVEL_INFO 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_CRITICAL 5

#define MAX_LOG 8
#define MAX_SIZE_MSG_LOG 64

#define SIZE_S_LOG 8+1

typedef struct logs {
	unsigned long long timeStamps;
	char priority;
	char log[MAX_SIZE_MSG_LOG];
} s_logs;

extern unsigned char gv_SelectedLogLevel;
extern char gvLogMsg[MAX_SIZE_MSG_LOG];

void logger(char level, char *message);
unsigned char getAllLogs(t_tc_response *resp);
unsigned char getLastLog(t_tc_response *resp);

#endif // LOG_H
