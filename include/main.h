#ifndef __MAIN_H
#define __MAIN_H

#include "common.h"
#include "ftp.h"

extern bool passive, debug, execution;
extern int sockfd, data_sockfd;
extern char * server_ip;

extern int yyparse(void);

#endif