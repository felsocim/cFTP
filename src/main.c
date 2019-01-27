#include "../include/ftp.h"
#include "../include/main.h"

char * help_message = "Simple FTP client.\nUsage: %s";
char * usage_message = "Simple FTP client.\nUsage: %s";

bool execution = true, debug = true;
int sockfd = 0;
char * server_ipaddr = NULL;
struct data_socket sdata;

int main(int argc, char ** argv) {
  if(argc != 1)
    usage(argv[0], EXIT_FAILURE);

  char * input = malloc(PROMPT_BUFFER_LENGTH);

  while(execution) {
    printf("hello\n");

    input = fgets(input, PROMPT_BUFFER_LENGTH, stdin);

    input[strlen(input) - 1] = '\0';

    printf("INPUT: %s\n", input);

    yy_scan_string(input);

    if(yyparse()) {
      execution = false;
    }

    printf("parsing done\n");

    yylex_destroy();
  }

  free(server_ipaddr);

  return 0;
}
