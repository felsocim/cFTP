#include "../include/ftp.h"
#include "../include/main.h"

bool passive = false, execution = true, debug = true;
int sockfd = 0, data_sockfd = 0;
char * server_ip = NULL;

int main(int argc, char ** argv) {
  if(argc != 1) {
    fprintf(stderr, "Arguments mismatch! Usage: %s\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char * input = malloc(PROMPT_BUFFER_LENGTH);

  printf("+-+ +-+ +-+ +-+\n"
         "|c| |F| |T| |P|\n"
         "+-+ +-+ +-+ +-+\n");

  printf("Welcome to a simple File Transfer Protocol (FTP) command line client for browsing and downloading files from FTP servers!\n"
         "Created by Marek Felsoci and Aurélien Rausch.\n"
         "Version %s.\n\n"
         "Type 'help' command to get help.\n\n", VERSION);  
 
  while(execution) {
    printf("cFTP> "); // show the prompt

    input = fgets(input, PROMPT_BUFFER_LENGTH, stdin);

    input[strlen(input) - 1] = '\0';

    yy_scan_string(input);

    if(yyparse()) {
      fprintf(stderr, "Unknown command '%s'!\n", input);
    }

    yylex_destroy();
  }

  free(server_ip);
  free(input);

  return 0;
}
