#ifndef __FTP_H
#define __FTP_H

#include "common.h"

#define PROMPT_BUFFER_LENGTH 1024
#define RECEIVER_BUFFER_SIZE 4096
#define PORT 21

void stdin_flush(void);
char * strip_first_last(char *);
bool is_error(const char *);
int open_connection(const char *, in_port_t, bool, bool);
int send_command(int, char *, char *);
char * recv_reply(int, FILE *, size_t);
int login(int, bool);
int passive_mode(int, const char *);
int active_mode(int, bool);
int close_connection(int, bool);

#endif