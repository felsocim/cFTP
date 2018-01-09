#include "../include/ftp.h"

int dir(int sockfd, char * directory) {
  char * buffer = malloc(MAX_COMMAND_LENGTH + MAX_ARGLIST_LENGTH + 2);
  char * receiver = malloc(RECEIVER_BUFFER_SIZE);
  char * command = "LIST ";

  if(buffer == NULL)
    failwith("Failed to allocate command buffer");

  if(receiver == NULL)
    failwith("Failed to allocate server response receiver");

  buffer = strcpy(buffer, command);

  if(buffer == NULL)
    failwith("Failed to construct command buffer (stage command)");

  buffer = strcat(buffer, directory);

  if(buffer == NULL)
    failwith("Failed to construct command buffer (stage argument)");

  if(send(sockfd, buffer, strlen(buffer), 0) == -1) {
    perror("Failed to communicate with FTP server! Check your network connection");
    return EXIT_FAILURE;
  }

  if(recv(sockfd, receiver, RECEIVER_BUFFER_SIZE, 0) == -1) {
    perror("Failed to receive server response");
    return EXIT_FAILURE;
  }

  printf("Response\n%s\n", receiver);

  free(buffer);
  free(receiver);

  return EXIT_SUCCESS;
}

void stdin_flush(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

int ftp_login_authenticate(int sock)
{
	struct termios term;
	char login[MAX_LOGIN_LEN];
	char passwd[MAX_PASSWD_LEN];
	char cmd[MAX_CMD_LEN];

	printf("username: ");
	fflush(stdout);
	scanf("%127[^\n]", login);
	stdin_flush();
	sprintf(cmd, "USER %s", login);

	if (send(sock, cmd, strlen(cmd), 0) == -1) {
		perror("send");
		return 1;
	}

	memset(cmd, '\0', sizeof(cmd));
	if (recv(sock, cmd, sizeof(cmd), 0) == -1) {
		perror("recv");
		return 1;
	}

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

	scanf("%255[^\n]", passwd);
	stdin_flush();

	sprintf(cmd, "PASS %s", passwd);
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

  char * receiver = malloc(RECEIVER_BUFFER_SIZE);

  if(recv(sock, receiver, RECEIVER_BUFFER_SIZE, 0) == -1) {
    perror("Failed to receive server response");
    return EXIT_FAILURE;
  }

  printf("Response\n%s\n", receiver);

  free(receiver);

	return sock;
err:
	close(sock);
	return -1;
}
