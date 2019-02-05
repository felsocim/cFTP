#include "../include/ftp.h"

void stdin_flush(void) {
	int c;
	while((c = getchar()) != '\n' && c != EOF);
}

char * strip_first_last(char * __s_to_consume) {
	size_t length = strlen(__s_to_consume);
	char * result = NULL;

	if(length < 3) {
		free(__s_to_consume);
		return NULL;
	}

	if(!(result = malloc(length - 1))) {
		return NULL;
	}

	length -= 2;
	
	char * temp = __s_to_consume + 1;

	size_t i = 0;

	for(; i < length; i++) {
		result[i] = temp[i];
	}

	result[length] = '\0';

	free(__s_to_consume);

	return result;
}

bool is_error(const char * response) {
	return response[0] == '4' || response[0] == '5';
}

int open_connection(const char * server_ip, in_port_t port, bool expect_reply) {
	struct sockaddr_in address;
	int sockfd;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	if(!inet_aton(server_ip, &address.sin_addr)) {
		goto error;
	}

	if(connect(sockfd, (struct sockaddr *) &address, sizeof(struct sockaddr)) == -1) {
		goto error;
	}

	if(expect_reply) {
		char * response;

		if(!(response = recv_reply(sockfd, NULL, 0)) || is_error(response)) {
			goto error;
		}
	}

	return sockfd;

error:
	close(sockfd);
	return -1;
}

int send_command(int sockfd, char * command, char * arguments) {
	if(!command) {
		return -1;
	}

	size_t length = strlen(command) + 3;

	if(arguments) {
		length += strlen(arguments) + 1;
	}

	char * line = malloc(length);

	if(!line) {
		return -1;
	}

	if(snprintf(line, length, "%s%s%s\r\n", command, arguments ? " " : "", arguments ? arguments : "") != length - 1) {
		return -1;
	}

	length -= 1;

  if(send(sockfd, line, length, 0) != length) {
    return -1;
  }

	return 0;
}

char * recv_reply(int sockfd, FILE * output, size_t expected_bytes) {
  ssize_t code = 0, total_size = 0;
  char * receiver = calloc(RECEIVER_BUFFER_SIZE, 1),
			 * total = NULL;

  do {
		memset(receiver, '\0', RECEIVER_BUFFER_SIZE);

    code = recv(sockfd, receiver, RECEIVER_BUFFER_SIZE - 1, 0);
 
		if(output) {
			if(fwrite(receiver, 1, code, output) != code) {
				free(receiver);
				return NULL;
			}

			total_size += code;
			printf("Acquiring file ... %lu%%\r", (((size_t) total_size * 100) / expected_bytes));
		} else {
			if(total_size < 1) {
				if(!(total = malloc(code + 1))) {
					free(receiver);
					return NULL;
				}

				total = strncpy(total, receiver, code);
				total_size = code;
			} else {
				total_size += code;

				if(!(total = realloc(total, total_size))) {
					free(receiver);
					return NULL;
				}

				total = strncat(total, receiver, code);
			}
		}
  } while(total_size < expected_bytes);

  if(code == -1) {
    return NULL;
  }

	if(output) {
		printf("Acquiring file ... 100%%\n");
	}

  free(receiver);

	return !output ? total : "";
}

int login(int sockfd, bool debug) {
	char * data;
	
	if(!(data = calloc(PROMPT_BUFFER_LENGTH, 1))) {
		return -1;
	}

	printf("cFTP> username: ");

	fflush(stdout);

	scanf("%s", data);

	stdin_flush();

	if(send_command(sockfd, "USER", data) == -1) {
		free(data);
		return -1;
	}

	char * response;

	if(!(response = recv_reply(sockfd, NULL, 0))) {
		free(data);
		return -1;
	}

	if(debug) {
		printf("%s\n", response);
	}

	free(data);

	if(is_error(response)) {
		free(response);
		return -1;
	}

	free(response);

	// PART 2: Password

	struct termios terminal;

	printf("cFTP> password (echo off): ");

	fflush(stdout);

	if(tcgetattr(0, &terminal) == -1) {
		return -1;
	}

	terminal.c_lflag &= ~ECHO;

	if(tcsetattr(1, TCSANOW, &terminal) == -1) {
		return -1;
	}

	if(!(data = calloc(PROMPT_BUFFER_LENGTH, 1))) {
		return -1;
	}

	if(!fgets(data, PROMPT_BUFFER_LENGTH, stdin)) {
		free(data);
		return -1;
	}

	printf("\n");

	if(send_command(sockfd, "PASS", data) == -1) {
		free(data);
		return -1;
	}

	terminal.c_lflag |= ECHO;

	if(tcsetattr(1, TCSANOW, &terminal) == -1) {
		free(data);
		return 1;
	}

	if(!(response = recv_reply(sockfd, NULL, 0))) {
		free(data);
		return -1;
	}

	if(debug) {
		printf("%s\n", response);
	}

	free(data);

	if(is_error(response)) {
		free(response);
		return -1;
	}

	free(response);

	return 0;
}

int passive_mode(int sockfd, const char * server_ip) {
	if(send_command(sockfd, "PASV", NULL)) {
		return -1;
	}

	char * response;

	if(!(response = recv_reply(sockfd, NULL, 0)) || is_error(response)) {
		free(response);
		return -1;
	}

	char * first = strchr(response, '(');

	int i1, i2, i3, i4, i5, i6;

	sscanf(first, "(%d,%d,%d,%d,%d,%d).\r\n", &i1, &i2, &i3, &i4, &i5, &i6);

	int data_socket;

	if((data_socket = open_connection(server_ip, i5 * 256 + i6, false)) == -1) {
		free(response);
		return -1;
	}

	free(response);
	return data_socket;
}

int active_mode(int sockfd) {
	struct sockaddr_in address;
	int data_sockfd;

	if((data_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = 0;

	if(bind(data_sockfd, (const struct sockaddr *) &address, sizeof(struct sockaddr)) == -1) {
		close(data_sockfd);
		return -1;
	}

	socklen_t length;

	if(getsockname(data_sockfd, (struct sockaddr *) &address, &length) == -1) {
		close(data_sockfd);
		return -1;
	}

	in_port_t port = ntohs(address.sin_port);
	char * local_ip;

	if(!(local_ip = malloc(INET_ADDRSTRLEN + 1))) {
		close(data_sockfd);
		return -1;
	}

	struct ifaddrs * interfaces, * iterator;

	if(getifaddrs(&interfaces) == -1) {
		free(local_ip);
		close(data_sockfd);
		
		return -1;
	}

	iterator = interfaces;

	while(iterator) {
		if((iterator->ifa_flags & IFF_UP) && !(iterator->ifa_flags & IFF_LOOPBACK) && iterator->ifa_addr->sa_family == AF_INET) {
			struct sockaddr_in * temp = (struct sockaddr_in *) iterator->ifa_addr;
			
			if(!(inet_ntop(AF_INET, &(temp->sin_addr), local_ip, INET_ADDRSTRLEN))) {
				freeifaddrs(interfaces);
				free(local_ip);
				close(data_sockfd);

				return -1;
			}

			break;
		}

		iterator = iterator->ifa_next;
	}

	freeifaddrs(interfaces);
	
	int i = 0;

	for(;i < INET_ADDRSTRLEN; i++) {
		if(local_ip[i] == '.') {
			local_ip[i] = ',';
		}
	}

	char * buffer;

	if(!(buffer = malloc(PROMPT_BUFFER_LENGTH))) {
		free(local_ip);
		close(data_sockfd);

		return -1;
	}

	if(snprintf(buffer, PROMPT_BUFFER_LENGTH, "%s,%d,%d", local_ip, (port / 256), port % 256) < 0) {
		free(local_ip);
		free(buffer);
		close(data_sockfd);

		return -1;
	}

	char * response;

	if(send_command(sockfd, "PORT", buffer) == -1 || !(response = recv_reply(sockfd, NULL, 0)) || is_error(response)) {
		free(local_ip);
		free(buffer);
		free(response);
		close(data_sockfd);

		return -1;
	}

	free(local_ip);
	free(buffer);
	free(response);

	return 0;
}

int close_connection(int sockfd) {
	char * response;

	if(send_command(sockfd, "QUIT", NULL) ||
			!(response = recv_reply(sockfd, NULL, 0)) ||
			is_error(response) ||
			close(sockfd) == -1
	) {
		return -1;
	}

	return 0;
}