#ifndef __MAIN_H
#define __MAIN_H

#include "common.h"
#include "ftp.h"

extern bool debug, execution;
extern int sockfd;
extern char * server_ipaddr;
extern struct data_socket sdata;

extern int yyparse(void);

#endif