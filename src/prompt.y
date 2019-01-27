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

%token OPEN CLOSE HELP EXIT LIST DELETE DIR GOTO GET PASSIVE DEBUG ON OFF
%token <string> NAME IPADDR

%%

command:
  OPEN IPADDR {
    server_ipaddr = strdup($2);

    if((sockfd = ftp_connect($2, PORT, debug)) == -1) {
      fprintf(stderr, "Failed to connect to %s!\n", $2);
    }

    if(ftp_login_authenticate(sockfd, debug)) {
      fprintf(stderr, "Login failed!\n");
    }

    if(ftp_passwd_authenticate(sockfd, debug)) {
      fprintf(stderr, "Login failed!\n");
    }

    /*if(ftp_data_socket(&sdata)) {
      fprintf(stderr, "Could not open the communication port!\n");
    }

    char * buffer = malloc(PROMPT_BUFFER_LENGTH);

    if(buffer == NULL) {
      fprintf(stderr, "Internal memory error!\n");
      YYABORT;
    }

    int i = 0;

    for(;i < INET_ADDRSTRLEN; i++)
      if(sdata.ip_addr[i] == '.')
        sdata.ip_addr[i] = ','; 

    if(snprintf(buffer, PROMPT_BUFFER_LENGTH, "PORT %s,%d,%d\r\n", sdata.ip_addr, (sdata.port / 256), sdata.port % 256)  < 0) {
      fprintf(stderr, "Internal memory error!\n");
      YYABORT;
    }

    printf("command: %s\n", buffer);

    if(send(sockfd, buffer, strlen(buffer), 0) < 0) {
      fprintf(stderr, "Failed to connect to %s!\n", $2);
      YYABORT;
    }

    if(debug) {
      receive(sockfd);
    }

    free(buffer);*/

    YYACCEPT;
  }
  | CLOSE {
    close(sockfd) ? fprintf(stderr, "Failed to close active connection!\n") : printf("Connection to %s closed.\n", server_ipaddr);

    YYACCEPT;
  }
  | HELP {

  }
  | EXIT {
    if(sockfd != 0) {
      close(sockfd) ? fprintf(stderr, "Failed to close active connection!\n") : printf("Connection to %s closed.\n", server_ipaddr);
    }

    execution = false;
    YYACCEPT;
  }
  | LIST NAME {
    dir(sockfd, $2, debug, sdata.sockfd) ? fprintf(stderr, "Could not get listing of: %s!\n", $2) : printf("Listing of %s\n", $2);

    YYACCEPT;
  }
  | DELETE NAME {

  }
  | DIR {
    char * buffer = malloc(PROMPT_BUFFER_LENGTH);

    if(buffer == NULL) {
      fprintf(stderr, "Internal memory error!\n");
      YYABORT;
    }

    if(snprintf(buffer, PROMPT_BUFFER_LENGTH, "PWD\r\n")  < 0) {
      fprintf(stderr, "Internal memory error!\n");
      YYABORT;
    }

    printf("command: %s\n", buffer);

    if(send(sockfd, buffer, strlen(buffer), 0) < 0) {
      fprintf(stderr, "Failed to connect to %s!\n", server_ipaddr);
      YYABORT;
    }

    if(debug) {
      receive(sockfd);
    }

    free(buffer);
  }
  | GOTO NAME {

  }
  | GET NAME {

  }
  | PASSIVE {
    char * buffer = malloc(PROMPT_BUFFER_LENGTH);

    if(buffer == NULL) {
      fprintf(stderr, "Internal memory error!\n");
      YYABORT;
    }

    if(snprintf(buffer, PROMPT_BUFFER_LENGTH, "PASV\r\n")  < 0) {
      fprintf(stderr, "Internal memory error!\n");
      YYABORT;
    }

    printf("command: %s\n", buffer);

    if(send(sockfd, buffer, strlen(buffer), 0) < 0) {
      fprintf(stderr, "Failed to send command to %s!\n", server_ipaddr);
      YYABORT;
    }

    char * response = recv_quiet(sockfd);

    printf("Response: %s\n", response);

    char * first = strchr(response, '(');

    int i1, i2, i3, i4, i5, i6;

    sscanf(first, "(%d,%d,%d,%d,%d,%d).\r\n", &i1, &i2, &i3, &i4, &i5, &i6);

    if((sdata.sockfd = ftp_connect(server_ipaddr, i5 * 256 + i6, false)) == -1) {
      fprintf(stderr, "Failed to establish data connection to %s!\n", server_ipaddr);
    }

    sdata.ip_addr = server_ipaddr;
    sdata.port = i5 * 256 + i6;

    free(buffer);
    free(response);
    YYACCEPT;
  }
  | DEBUG ON {
    debug = true;
    YYACCEPT;
  }
  | DEBUG OFF {
    debug = false;
    printf("debug off\n");
    YYACCEPT;
  };
