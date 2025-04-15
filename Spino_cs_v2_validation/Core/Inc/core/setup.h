#ifndef SETUP_H
#define SETUP_H

#include "configuration.h"
#include "../ax25/ax25.h"
#include "command.h"
#include "../logMngt/log.h"

#define  SPINO_CMD_KEY 0x0FF0



extern unsigned short gv_spino_cmd_key;

extern s_configuration_spino gv_spinoConfig;
extern s_globalVariable gv_spino;
extern s_ax25_header gv_headerTlm;
extern unsigned short gv_version;
extern s_prog_mngt gv_prog;
extern char memory_reprog[];
void setupGlobalVariable();

#endif  // SETUP_H
