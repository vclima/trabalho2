
%{
#include <stdlib.h>
#include <string.h>
void yyerror(char *);

#include "y.tab.h"
%}



%%
"EXIT"|"Exit"|"exit" {
  return EXIT;
}

[a-z]  {
        yylval.ival = yytext[0] - 'a';
        return VAR;
       }
[0-9\.]+ {
    yylval.fval=atof(yytext);
    printf("Achei o valor %.2f\n",yylval.fval);
    return NUMERO;
}

\*   {
    return BRILHO_MULT;
}
[\\/]   {
    return BRILHO_DIV;
}

[\[\]]  {
    return COLCHETE;
}

[a-zA-Z0-9\.]+  {
        strcpy(yylval.strval,yytext);
        printf ("Achei nome: %s\n", yylval.strval);
        return STRING;
       }

[=]     {
          return IGUAL;
        }

\n   {
         return EOL;
       }

[ \t]+ ;

.     {printf("**%s**\n", yytext);
yyerror("Token nao reconhecido!\n");}

%%

int  yywrap() {
  return 1;
}
