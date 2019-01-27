#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>

// Should hold a help message string which will be displayed anytime the 'help'
// option is present
extern char * help_message;
// Should hold a usage message string which will be displayed anytime the
// application encounters a syntax error on startup
extern char * usage_message;

void usage(const char * arg_0, const int exit_code);

#endif
