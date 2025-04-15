
#ifndef CONTROL_H
#define CONTROL_H

#define STATE_INIT 0
#define STATE_SURVEY 1
#define STATE_MAILBOX 2
#define STATE_DIGIPEATER 3
#define STATE_EXPE_DATA 4
#define STATE_MAIN_PAYLOAD 5

unsigned char changeState(int state);
void control();
void inittlm();
void sendTLMWithoutDelay();
#endif // CONTROL_H
