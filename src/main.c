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

  bool execution = true;
  int sockfd = 0;

  while(execution) {
    if(scanf("%s %s", command, arglist) == EOF)
      failwith("User input error");

    if(strcmp(command, COMMAND_EXIT) == 0) {
      execution = false;
      continue;
    }

    if(strcmp(command, COMMAND_CONNECT) == 0) {
      sockfd = ftp_connect(arglist);
      if(sockfd == -1) {
        fprintf(stderr, "Connection failed\n");
      }
      ftp_login_authenticate(sockfd);
      ftp_passwd_authenticate(sockfd);
      printf("dcjdolcjds\n");
      continue;
    }

    if(strcmp(command, COMMAND_DIRECTORY_LIST) == 0) {
      if(dir(sockfd, arglist) != EXIT_SUCCESS) {
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
