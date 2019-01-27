#ifndef __FTP_H
#define __FTP_H

#include "common.h"

#define PROMPT_BUFFER_LENGTH 1024
#define RECEIVER_BUFFER_SIZE 4096
#define MAX_LOGIN_LEN	128
#define MAX_PASSWD_LEN	256
#define PORT 		21

struct data_socket {
	int sockfd;
	char * ip_addr;
	unsigned short port;
};

int dir(int, char *, bool, int);
int show(int, char *, bool, int);
void receive(int);
char * recv_quiet(int);
void stdin_flush(void);
int ftp_login_authenticate(int, bool);
int ftp_passwd_authenticate(int, bool);
int ftp_connect(const char *, in_port_t, bool);
int ftp_data_socket(struct data_socket *dsock);

#endif