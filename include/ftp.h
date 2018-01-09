#include "common.h"

#define RECEIVER_BUFFER_SIZE 1024
#define MAX_CMD_LEN	4096
#define MAX_LOGIN_LEN	128
#define MAX_PASSWD_LEN	256
#define PORT 		21

struct data_socket {
	int sockfd;
	unsigned short port;
};

int dir(int, char *);
int show(int, char *);
void receive(int);
void stdin_flush(void);
int ftp_login_authenticate(int);
int ftp_passwd_authenticate(int);
int ftp_connect(const char *);
int ftp_data_socket(struct data_socket *dsock);
