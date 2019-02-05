#ifndef __MAIN_H
#define __MAIN_H

#include "common.h"
#include "ftp.h"
#include "lexer.h"
#include "prompt.h"

extern bool passive, debug, execution;
extern int sockfd, data_sockfd;
extern char * server_ip;

#endif