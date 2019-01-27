#include "../include/ftp.h"

int ftp_data_socket(struct data_socket *dsock)
{
	struct sockaddr_in addr;
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd == -1) {
		perror("socket");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 0;

	if (bind(fd, (const struct sockaddr *)&addr, sizeof(struct sockaddr))) {
		perror("bind");
		close(fd);
		return 1;
	}

	socklen_t alen;

	getsockname(fd, (struct sockaddr *)&addr, &alen);

	printf("Port num af bind: %u\n", ntohs(addr.sin_port));

	dsock->sockfd = fd;
	dsock->port = ntohs(addr.sin_port);
	dsock->ip_addr = (char*)malloc(INET_ADDRSTRLEN);

	struct ifaddrs * interfaces, * iterator;

	getifaddrs(&interfaces);

	iterator = interfaces;

	while(iterator) {
		if((iterator->ifa_flags & IFF_UP) && !(iterator->ifa_flags & IFF_LOOPBACK) && iterator->ifa_addr->sa_family == AF_INET) {
			struct sockaddr_in * temp = (struct sockaddr_in *) iterator->ifa_addr;
			inet_ntop(AF_INET, &(temp->sin_addr), dsock->ip_addr, INET_ADDRSTRLEN);
			break;
		}
		iterator = iterator->ifa_next;
	}

	freeifaddrs(interfaces);

	return 0;
}

int dir(int sockfd, char * directory, bool debug, int datasock) {
  char * command = malloc(PROMPT_BUFFER_LENGTH);
  
	if(snprintf(command, PROMPT_BUFFER_LENGTH, "LIST\r\n"/*, directory*/) < 0) {
		fprintf(stderr, "Internal memory error!\n");
		return EXIT_FAILURE;
	}

  if(send(sockfd, command, strlen(command), 0) < 0) {
    fprintf(stderr, "Failed to send your request!\n");
    return EXIT_FAILURE;
  }

	printf("Sent LIST\n");

  if(debug) {
    receive(sockfd);
  }

	printf("About to receive data\n");

  receive(datasock);

	printf("Data rcv end, waiting for control\n");

	if(debug) {
    receive(sockfd);
  }

	printf("control rcvd\n");

  free(command);

  return EXIT_SUCCESS;
}

void receive(int sockfd) {
  int code = 0;
  char * receiver = malloc(RECEIVER_BUFFER_SIZE);

  do {
    code = recv(sockfd, receiver, RECEIVER_BUFFER_SIZE, 0);
    printf("%s\n", receiver);
    memset(receiver, '\0', RECEIVER_BUFFER_SIZE);
  } while(code == RECEIVER_BUFFER_SIZE);

  if(code == -1) {
    perror("Failed to receive server response");
    return;
  }

  free(receiver);
}

char * recv_quiet(int sockfd) {
  ssize_t code = 0;
  char * receiver = malloc(RECEIVER_BUFFER_SIZE);
	char * total = NULL;
	ssize_t total_size = 0;

  do {
    code = recv(sockfd, receiver, RECEIVER_BUFFER_SIZE, 0);
    
		if(total_size < 1) {
			total = malloc(code);
			total = strncpy(total, receiver, code);
			total_size = code;
		} else {
			total_size += code;
			total = realloc(total, total_size);
			total = strncat(total, receiver, code);
		}

    memset(receiver, '\0', RECEIVER_BUFFER_SIZE);
  } while(code == RECEIVER_BUFFER_SIZE);

  if(code == -1) {
    perror("Failed to receive server response");
    return NULL;
  }

	total = realloc(total, total_size + 1);
	total[total_size] = '\0';

  free(receiver);

	return total;
}

int show(int sockfd, char * file, bool debug, int datasock) {
  char * command = malloc(PROMPT_BUFFER_LENGTH);

	if(snprintf(command, PROMPT_BUFFER_LENGTH, "RETR %s\r\n", file) < 0) {
		fprintf(stderr, "Internal memory error!\n");
		return EXIT_FAILURE;
	}

  if(send(sockfd, command, strlen(command), 0) == -1) {
    fprintf(stderr, "Failed to send your request!\n");
    return EXIT_FAILURE;
  }

  if(debug) {
    receive(sockfd);
  }

  receive(datasock);

  free(command);

  return EXIT_SUCCESS;
}

void stdin_flush(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

int ftp_login_authenticate(int sock, bool debug)
{
	char login[MAX_LOGIN_LEN];
	char cmd[PROMPT_BUFFER_LENGTH];

	printf("username: ");
	fflush(stdout);
	scanf("%s", login);
	stdin_flush();
	sprintf(cmd, "USER %s\r\n", login);

	if (send(sock, cmd, strlen(cmd), 0) == -1) {
		perror("send");
		return 1;
	}

	memset(cmd, '\0', sizeof(cmd));
	if (recv(sock, cmd, sizeof(cmd), 0) == -1) {
		perror("recv");
		return 1;
	}
	/* TODO: gérer erreur (bad login) */
  if(debug) {
    printf("%s\n", cmd);
  } else {
    printf("OK\n");
  }

	return 0;
}

int ftp_passwd_authenticate(int sock, bool debug)
{
	struct termios term;
	char passwd[MAX_PASSWD_LEN];
	char cmd[PROMPT_BUFFER_LENGTH];

	printf("password (echo disabled): ");
	fflush(stdout);

	if (tcgetattr(0, &term) == -1) {
		perror("tcgetattr");
		return 1;
	}

	term.c_lflag &= ~ECHO;
	if (tcsetattr(1, TCSANOW, &term) == -1) {
		perror("tcsetattr");
		return 1;
	}

	memset(passwd, '\0', sizeof(passwd));
	fgets (passwd, MAX_PASSWD_LEN, stdin);
	passwd[strlen(passwd) - 1] = '\0';

	sprintf(cmd, "PASS %s\r\n", passwd);
	if (send(sock, cmd, strlen(cmd), 0) == -1) {
		perror("send");
		return 1;
	}

	term.c_lflag |= ECHO;
	if (tcsetattr(1, TCSANOW, &term) == -1) {
		perror("tcsetattr");
		return 1;
	}

	memset(cmd, '\0', sizeof(cmd));
	if (recv(sock, cmd, sizeof(cmd), 0) == -1) {
		perror("recv");
		return 1;
	}
	/* TODO: gérer erreur (bad password) */
  if(debug) {
    printf("%s\n", cmd);
  } else {
    printf("OK\n");
  }

	return 0;
}

int ftp_connect(const char *ip, in_port_t port, bool debug)
{
	int err;
	struct sockaddr_in addr;
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1) {
		perror("socket");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (inet_aton(ip, &addr.sin_addr) == 0) {
		fprintf(stderr, "Invalid address\n");
		goto err;
	}

	err = connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr));
	if (err) {
		perror("connect");
		goto err;
	}

  if(debug) {
    receive(sock);
  } else {
    printf("Connection OK\n");
  }

	return sock;
err:
	close(sock);
	return -1;
}
