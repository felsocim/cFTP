#include "../include/ftp.h"

int ftp_data_socket(struct data_socket *dsock)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd == -1) {
		perror("socket");
		return -1;
	}

	return 0;
}

int dir(int sockfd, char * directory) {
  char * buffer = malloc(MAX_COMMAND_LENGTH + MAX_ARGLIST_LENGTH + 4);
  char * command = "LIST ";

  if(buffer == NULL)
    failwith("Failed to allocate command buffer");

  buffer = strcpy(buffer, command);

  if(buffer == NULL)
    failwith("Failed to construct command buffer (stage command)");

  buffer = strcat(buffer, directory);

  if(buffer == NULL)
    failwith("Failed to construct command buffer (stage argument 1)");

  buffer = strcat(buffer, "\r\n");

  if(buffer == NULL)
    failwith("Failed to construct command buffer (stage argument 2)");

  if(send(sockfd, buffer, strlen(buffer), 0) == -1) {
    perror("Failed to communicate with FTP server! Check your network connection");
    return EXIT_FAILURE;
  }

  receive(sockfd);

  free(buffer);

  return EXIT_SUCCESS;
}

void receive(int sockfd) {
  int code = 0;
  char * receiver = malloc(RECEIVER_BUFFER_SIZE);

  do {
    code = recv(sockfd, receiver, RECEIVER_BUFFER_SIZE, 0);
    printf("Response\n%s\n", receiver);
    memset(receiver, '\0', RECEIVER_BUFFER_SIZE);
  } while(code == RECEIVER_BUFFER_SIZE);

  if(code == -1) {
    perror("Failed to receive server response");
    return;
  }

  free(receiver);
}

int show(int sockfd, char * file) {
  char * buffer = malloc(MAX_COMMAND_LENGTH + MAX_ARGLIST_LENGTH + 4);
  char * command = "RETR ";

  if(buffer == NULL)
    failwith("Failed to allocate command buffer");

  buffer = strcpy(buffer, command);

  if(buffer == NULL)
    failwith("Failed to construct command buffer (stage command)");

  buffer = strcat(buffer, file);

  if(buffer == NULL)
    failwith("Failed to construct command buffer (stage argument 1)");

  buffer = strcat(buffer, "\r\n");

  printf("BUFF=%s\n", buffer);

  if(buffer == NULL)
    failwith("Failed to construct command buffer (stage argument 2)");

  if(send(sockfd, buffer, strlen(buffer), 0) == -1) {
    perror("Failed to communicate with FTP server! Check your network connection");
    return EXIT_FAILURE;
  }

  receive(sockfd);

  free(buffer);

  return EXIT_SUCCESS;
}

void stdin_flush(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

int ftp_login_authenticate(int sock)
{
	char login[MAX_LOGIN_LEN];
	char cmd[MAX_CMD_LEN];

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
	printf("%s\n", cmd);
	return 0;
}

int ftp_passwd_authenticate(int sock)
{
	struct termios term;
	char passwd[MAX_PASSWD_LEN];
	char cmd[MAX_CMD_LEN];

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

	scanf("%s", passwd);
	stdin_flush();

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
	printf("%s\n", cmd);

	return 0;
}

int ftp_connect(const char *ip)
{
	int err;
	struct sockaddr_in addr;
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1) {
		perror("socket");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);

	if (inet_aton(ip, &addr.sin_addr) == 0) {
		fprintf(stderr, "Invalid address\n");
		goto err;
	}

	err = connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr));
	if (err) {
		perror("connect");
		goto err;
	}

  receive(sock);

	return sock;
err:
	close(sock);
	return -1;
}
