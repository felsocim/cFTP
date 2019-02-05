%{
  #include "../include/common.h"
  #include "../include/ftp.h"
  #include "../include/main.h"

  void yyerror(char *);
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

    if((sockfd = open_connection(server_ip, PORT, true, debug)) == -1 || login(sockfd, debug) == -1) {
      fprintf(stderr, "Unable to connect to '%s'!\n\n", server_ip);
    }

    free($2);
    YYACCEPT;
  }
  | CLOSE {
    if(close_connection(sockfd, debug) == -1) {
      fprintf(stderr, "Unable to close connection to '%s'!\n\n", server_ip);
    } else {
      sockfd = 0;
      printf("Connection to '%s' closed!\n\n", server_ip);
    }
    
    YYACCEPT;
  }
  | HELP {
    printf("This is a simple File Transfer Protocol (FTP) command line client.\n\n"
           "See the list of available commands and their syntax below:\n"
           "\t- open IPADDR\tOpens new connection to the FTP server identified by the IPv4 address IPADDR.\n"
           "\t- close\t\tCloses currently opened connection.\n"
           "\t- help\t\tShows this help message.\n"
           "\t- exit\t\tCloses currently opened connection and quits the application.\n"
           "\t- list \"PATH\"\tGets listing of the directory under the path PATH.\n"
           "\t- delete \"PATH\"\tDeletes the file under the path PATH from the server without any confirmation.\n"
           "\t- dir\t\tGets path of the current working directory on the server.\n"
           "\t- goto \"PATH\"\tNavigates to the path PATH on the server.\n"
           "\t- get \"PATH\"\tRetrieves the file under the path PATH from the server onto local computer.\n"
           "\t- passive\tEnters passive FTP mode.\n"
           "\t- active\tEnters active FTP mode.\n"
           "\t- debug on|off\tShows/hides debugging informations on/from the standard output.\n\n"
           "This application was developed within our studies at the University of Strasbourg and is licensed under the terms of the MIT license (see LICENSE file for the full license text).\n\n"
           "More information and source code of the application can be found at <https://github.com/felsocim/cFTP>.\n\n"
           "Copyright (C) 2018 - 2019 Marek Felsoci and Aurélien Rausch\n");
    
    YYACCEPT;
  }
  | EXIT {
    if(sockfd != 0) {
      if(close_connection(sockfd, debug) == -1) {
        fprintf(stderr, "Unable to close connection to '%s'! Force quit? [YyNn]\n", server_ip);
        
        char reply = fgetc(stdin);
        
        if(reply == 'Y' || reply == 'y') {
          sockfd = 0;
        }
      } else {
        sockfd = 0;
        printf("Connection to '%s' closed!\n\n", server_ip);
      }
    }
    
    execution = sockfd;
    YYACCEPT;
  }
  | LIST NAME {
    if(passive) {
      if((data_sockfd = passive_mode(sockfd, server_ip)) == -1) {
        fprintf(stderr, "Failed to sent your request in passive mode! Try to switch to active mode.\n\n");
        data_sockfd = 0;

        YYACCEPT;
      }
    } else {
      if((data_sockfd = active_mode(sockfd, debug)) == -1) {
        fprintf(stderr, "Failed to sent your request in active mode! Try to switch to passive mode.\n\n");
        data_sockfd = 0;

        YYACCEPT;
      }
    }

    char * target = strip_first_last($2),
         * response = NULL;
    bool error = false;

    if(send_command(sockfd, "LIST", target) != -1 &&
        (response = recv_reply(sockfd, NULL, 0)) &&
        !is_error(response)
    ) {
      if(debug) {
        printf("%s\n", response);
      }

      free(response);

      if((response = recv_reply(data_sockfd, NULL, 0)) &&
          !is_error(response)
      ) {
        printf("%s\n", response);

        free(response);

        if(!(response = recv_reply(sockfd, NULL, 0)) ||
           is_error(response)
        ) {
          error = true;
        }
      } else {
        error = true;
      }
    } else {
      error = true;
    }
  
    if(response) {
      if(debug) {
        printf("%s\n", response);
      } 

      free(response);
    }

    if(error) {
      fprintf(stderr, "Could not get listing of '%s'!\n\n", target);
    }

    free(target);
    YYACCEPT;
  }
  | DELETE NAME {
    char * target = strip_first_last($2),
         * response;
    bool error = (send_command(sockfd, "DELE", target) == -1 ||
                 !(response = recv_reply(sockfd, NULL, 0)) ||
                 is_error(response));

    if(response) {
      if(debug) { 
        printf("%s\n", response);
      }
      
      free(response);
    }

    if(error) {
      fprintf(stderr, "Could not delete file '%s'!\n\n", target);
    }

    free(target);
    YYACCEPT;
  }
  | DIR {
    char * response;
    bool error = (send_command(sockfd, "PWD", NULL) == -1 ||
                 !(response = recv_reply(sockfd, NULL, 0)) ||
                 is_error(response));
                 
    if(response) {
      printf("%s\n", response);      
      free(response);
    }
    
    if(error) {
      fprintf(stderr, "Could not get path to the current working directory!\n\n");
    }

    YYACCEPT;
  }
  | GOTO NAME {
    char * target = strip_first_last($2),
         * response;
    bool error = (send_command(sockfd, "CWD", target) == -1 ||
                 !(response = recv_reply(sockfd, NULL, 0)) ||
                 is_error(response));

    if(response) {
      if(debug) { 
        printf("%s\n", response);
      }
      
      free(response);
    }
                 
    if(error) {
      fprintf(stderr, "Could not change working directory to '%s'!\n\n", target);
    }

    free(target);
    YYACCEPT;
  }
  | GET NAME {
    if(passive) {
      if((data_sockfd = passive_mode(sockfd, server_ip)) == -1) {
        fprintf(stderr, "Failed to sent your request in passive mode! Try to switch to active mode.\n\n");
        data_sockfd = 0;

        YYACCEPT;
      }
    } else {
      if((data_sockfd = active_mode(sockfd, debug)) == -1) {
        fprintf(stderr, "Failed to sent your request in active mode! Try to switch to passive mode.\n\n");
        data_sockfd = 0;

        YYACCEPT;
      }
    }

    char * target = strip_first_last($2),
         * response;
    FILE * output;
    bool error = false;

    if(send_command(sockfd, "RETR", target) != -1 && (output = fopen(basename(target), "w+"))) {
      if((response = recv_reply(sockfd, NULL, 0)) && !is_error(response)) {
        if(debug) { 
          printf("%s\n", response);
        }

        size_t size = 0;
        char * temp = strchr(response, '(');

        if(temp && sscanf(temp, "(%lu bytes).\r\n", &size) != EOF) {
          if(!recv_reply(data_sockfd, output, size) || fclose(output) == EOF) {
            error = true;
          }
        } else {
          error = true;
        }
      } else {
        error = true;
      }
    } else {
      error = true;
    }

    if(response) {
      free(response);
    }

    if(error) {
      fprintf(stderr, "Could not retrieve file '%s'!\n\n", target);
    }

    free(target);

    YYACCEPT;
  }
  | PASSIVE {
    passive = true;
    printf("Entered passive mode.\n\n");
    YYACCEPT;
  }
  | ACTIVE {
    passive = false;
    printf("Entered active mode.\n\n");
    YYACCEPT;
  }
  | DEBUG ON {
    debug = true;
    printf("Debugging information will be shown.\n\n");
    YYACCEPT;
  }
  | DEBUG OFF {
    debug = false;
    printf("Debugging information will be hidden.\n\n");
    YYACCEPT;
  };
