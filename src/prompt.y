%{
  #include "../include/common.h"
  #include "../include/ftp.h"
  #include "../include/main.h"

  int yylex();
  void yyerror(char*);
%}

%union {
  char * string;
}

%token OPEN CLOSE HELP EXIT LIST DELETE DIR GOTO GET PASSIVE ACTIVE DEBUG ON OFF
%token <string> NAME IPADDR

%%

command:
  OPEN IPADDR {
    server_ip = strdup($2);

    if((sockfd = open_connection(server_ip, PORT)) == -1 || login(sockfd, debug) == -1) {
      fprintf(stderr, "Unable to connect to '%s'!\n", server_ip);
    }

    free($2);
    YYACCEPT;
  }
  | CLOSE {
    if(close_connection(sockfd) == -1) {
      fprintf(stderr, "Unable to close connection to '%s'!\n", server_ip);
    } else {
      sockfd = 0;
      printf("Connection to '%s' closed!\n", server_ip);
    }
    
    YYACCEPT;
  }
  | HELP {

  }
  | EXIT {
    if(sockfd != 0) {
      if(close_connection(sockfd) == -1) {
        fprintf(stderr, "Unable to close connection to '%s'!\n", server_ip);
        YYACCEPT;
      } else {
        sockfd = 0;
        printf("Connection to '%s' closed!\n", server_ip);
      }
    }
    
    execution = sockfd;
    YYACCEPT;
  }
  | LIST NAME {
    if(passive) {
      if((data_sockfd = passive_mode(sockfd, server_ip)) == -1) {
        fprintf(stderr, "Failed to sent your request in passive mode! Try to switch to active mode.\n");
        data_sockfd = 0;

        YYACCEPT;
      }
    } else {
      if((data_sockfd = active_mode(sockfd)) == -1) {
        fprintf(stderr, "Failed to sent your request in active mode! Try to switch to passive mode.\n");
        data_sockfd = 0;

        YYACCEPT;
      }
    }

    char * target = strip_first_last($2),
         * response;

    if(send_command(sockfd, "LIST", target) == -1 ||
        !(response = recv_reply(sockfd, NULL)) ||
        is_error(response) ||
        !(response = recv_reply(data_sockfd, NULL)) ||
        is_error(response)
    ) {
      fprintf(stderr, "Could not get listing of '%s'!\n", target);
    }

    if(response) {
      free(response);
    }

    free(target);
    YYACCEPT;
  }
  | DELETE NAME {
    char * target = strip_first_last($2),
         * response;

    if(send_command(sockfd, "DELE", target) == -1 ||
        !(response = recv_reply(sockfd, NULL)) ||
        is_error(response)
    ) {
      fprintf(stderr, "Could not delete file '%s'!\n", target);
    }

    if(response) {
      free(response);
    }

    free(target);
    YYACCEPT;
  }
  | DIR {
    char * response;

    if(send_command(sockfd, "PWD", NULL) == -1 ||
        !(response = recv_reply(sockfd, NULL)) ||
        is_error(response)
    ) {
      fprintf(stderr, "Could not get path to the current working directory!\n");
    }

    if(response) {
      free(response);
    }

    YYACCEPT;
  }
  | GOTO NAME {
    char * target = strip_first_last($2),
         * response;

    if(send_command(sockfd, "CWD", target) == -1 ||
        !(response = recv_reply(sockfd, NULL)) ||
        is_error(response)
    ) {
      fprintf(stderr, "Could not change working directory to '%s'!\n", target);
    }

    if(response) {
      free(response);
    }

    free(target);
    YYACCEPT;
  }
  | GET NAME {
    if(passive) {
      if((data_sockfd = passive_mode(sockfd, server_ip)) == -1) {
        fprintf(stderr, "Failed to sent your request in passive mode! Try to switch to active mode.\n");
        data_sockfd = 0;

        YYACCEPT;
      }
    } else {
      if((data_sockfd = active_mode(sockfd)) == -1) {
        fprintf(stderr, "Failed to sent your request in active mode! Try to switch to passive mode.\n");
        data_sockfd = 0;

        YYACCEPT;
      }
    }

    char * target = strip_first_last($2),
         * response;
    FILE * output;

    if(send_command(sockfd, "RETR", target) == -1 ||
        !(output = fopen(basename(target), "w+")) ||
        !(response = recv_reply(sockfd, NULL)) ||
        is_error(response) ||
        !recv_reply(data_sockfd, output) ||
        fclose(output) == EOF
    ) {
      fprintf(stderr, "Could not retrieve file '%s'!\n", target);
    }

    free(target);
    YYACCEPT;
  }
  | PASSIVE {
    passive = true;
    YYACCEPT;
  }
  | ACTIVE {
    passive = false;
    YYACCEPT;
  }
  | DEBUG ON {
    debug = true;
    YYACCEPT;
  }
  | DEBUG OFF {
    debug = false;
    YYACCEPT;
  };
