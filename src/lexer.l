%{
  #include "../include/common.h"
  #include "../include/prompt.h"
%}

%option nounput
%option noyywrap
%option noinput

/* Regular expressions to recognize */
/*  - file or domain name */
name \"[^\?\:\!\*]+\"
/*  - IPv4 address*/
ipaddr [0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}

%%

open { return OPEN; }
close { return CLOSE; }
help { return HELP; }
exit { return EXIT; }
list { return LIST; }
delete { return DELETE; }
dir { return DIR; }
goto { return GOTO; }
get { return GET; }
passive { return PASSIVE; }
active { return ACTIVE; }
debug { return DEBUG; }
on { return ON; }
off { return OFF; }

{name} { yylval.string = strdup(yytext); return NAME; }
{ipaddr} { yylval.string = strdup(yytext); return IPADDR; }

[ ] ;

. { fprintf(stderr, "Unexpected token at '%s'!\n", yytext); } { return EXIT_FAILURE; }
