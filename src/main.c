#include "../include/ftp.h"
#include "../include/main.h"

char * help_message = "Simple FTP client.\nUsage: %s";
char * usage_message = "Simple FTP client.\nUsage: %s";

bool passive = false, execution = true, debug = true;
int sockfd = 0, data_sockfd = 0;
char * server_ip = NULL;

int main(int argc, char ** argv) {
  if(argc != 1)
    usage(argv[0], EXIT_FAILURE);

  char * input = malloc(PROMPT_BUFFER_LENGTH);

  printf("Welcome to the simple FTP client!\n");

  while(execution) {
    printf("cFTP> ");
    input = fgets(input, PROMPT_BUFFER_LENGTH, stdin);

    input[strlen(input) - 1] = '\0';

    yy_scan_string(input);

    if(yyparse()) {
      fprintf(stderr, "Unknown command '%s'!\n", input);
    }

    yylex_destroy();
  }

  free(server_ip);

  return 0;
}
