%{
#include "diagram.h"
#undef BITS		/* prevent conflict with yacc macro */
%}

%token NUMBER
%token DONE

%%
line		: diagram DONE		{ return 0; }
		;
diagram		: mdiagram
		| sdiagram
		;
mdiagram	: motif
		| motif '-' diagram
		;
sdiagram	: spacer
		| spacer '-' mdiagram
		;
motif		: '[' NUMBER ']'	
					{ order[norder++] = $2; }
		| '<' NUMBER '>'
					{ order[norder++] = $2; }
		;
spacer		: NUMBER		{ space[norder] = $1; }
		;

%%
/* read an integer of single character from the buffer named "diagram" */
int yylex() {
  int c;
  int is_number = 0;
  int number = 0;

  /* parse the token */
  while (((c = diagram[dptr++]) != '\0') && isdigit(c)) {
    number = 10*number + c-'0';
    is_number = 1;
    /*printf("dptr %d c %c\n", dptr, c);*/
  }

  /* return the token */
  if (is_number) {
    dptr--;			/* put back non-number terminator */
    yylval = number;
    /*printf("NUMBER: returning %d\n", number);*/
    return NUMBER;
  } else if (c == '\0') {
    /*printf("DONE: returning %d\n", DONE);*/
    return DONE;
  } else {
    /*printf("returning %d\n", c);*/
    return c;
  }
}

void yyerror(char *s)
{
  printf("Error parsing block diagram:\n  %s\n", diagram);
}
