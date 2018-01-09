#include "../include/ftp.h"

char * help_message = "Simple FTP client.\nUsage: %s";
char * usage_message = "Simple FTP client.\nUsage: %s";

int main(int argc, char ** argv) {
  if(argc != 1)
    usage(argv[0], EXIT_FAILURE);

  char * command = malloc(MAX_COMMAND_LENGTH);

  if(command == NULL)
    failwith("Failed to allocate memory for command string");

  char * arglist = malloc(MAX_ARGLIST_LENGTH);

  if(arglist == NULL)
    failwith("Failed to allocate memory for argument list string");

  bool execution = true, debug = false;
  int sockfd = 0;
  struct data_socket sdata;

  while(execution) {
    if(scanf("%s %s", command, arglist) == EOF)
      failwith("User input error");

    if(strcmp(command, COMMAND_EXIT) == 0) {
      execution = false;
      continue;
    }

    if(strcmp(command, COMMAND_DISCONNECT) == 0) {
      close(sockfd);
      printf("Connection closed\n");
      continue;
    }

    if(strcmp(command, COMMAND_DEBUGON) == 0) {
      debug = true;
      continue;
    }

    if(strcmp(command, COMMAND_DEBUGOFF) == 0) {
      debug = false;
      continue;
    }

    if(strcmp(command, COMMAND_CONNECT) == 0) {
      sockfd = ftp_connect(arglist, debug);
      if(sockfd == -1) {
        fprintf(stderr, "Connection failed\n");
      }
      ftp_login_authenticate(sockfd, debug);
      ftp_passwd_authenticate(sockfd, debug);
      if(ftp_data_socket(&sdata) == 1) {
        fprintf(stderr, "Data port opening failed\n");
      }
      continue;
    }

    if(strcmp(command, COMMAND_DIRECTORY_LIST) == 0) {
      if(dir(sockfd, arglist, debug) != EXIT_SUCCESS) {
        printf("Are you connected?\n");
      }
      continue;
    }

    if(strcmp(command, COMMAND_VIEW_FILE) == 0) {
      if(show(sockfd, arglist, debug) != EXIT_SUCCESS) {
        printf("Are you connected?\n");
      }
      continue;
    }

    fprintf(stderr, "Unable to interpret command '%s'! Check your syntax.\n", command);
  }

  free(command);
  free(arglist);

  return 0;
}
