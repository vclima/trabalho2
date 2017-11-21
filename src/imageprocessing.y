%{
#include <stdio.h>
#include "imageprocessing.h"
#include <FreeImage.h>

void yyerror(char *c);
int yylex(void);

%}
%union {
  char    strval[50];
  int     ival;
  float   fval;
}
%token <strval> STRING
%token <ival> VAR IGUAL EOL BRILHO_MULT BRILHO_DIV COLCHETE EXIT
%token <fval> NUMERO
%left SOMA

%%

PROGRAMA:
        PROGRAMA EXPRESSAO EOL
        |
        ;

EXPRESSAO:
    | STRING IGUAL STRING {
        printf("Copiando %s para %s\n", $3, $1);
        imagem I = abrir_imagem($3);
        printf("Li imagem %d por %d\n", I.width, I.height);
        salvar_imagem($1, &I);
        liberar_imagem(&I);
    }
    |STRING BRILHO_MULT NUMERO{
      printf("Modificando o brilho de %s com um fator de %.2f\n",$1,$3);
      imagem I1,I2,I3;
      imagem I = abrir_imagem($1);
      I=brilho_thread(I,$3,4);
      I1=brilho_process(I,$3,4);
      I2=brilho_h(I,$3);
      I3=brilho_v(I,$3);
      salvar_imagem($1,&I);
      liberar_imagem(&I);
    }
      |STRING BRILHO_DIV NUMERO{
      printf("Modificando o brilho de %s com um fator de %.2f\n",$1,1/$3);
      imagem I = abrir_imagem($1);
      I=brilho_v(I,1/$3);
      salvar_imagem($1,&I);
      liberar_imagem(&I);
    }
      |COLCHETE STRING COLCHETE {
        float max;
        imagem I = abrir_imagem($2);
        max=maximo(I);
        printf("Maximo de %s: %.2f\n",$2,max );
    }
      |EXIT {
        printf("Saindo\n");
        return 0;
      }

    ;

%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main() {
  FreeImage_Initialise(0);
  yyparse();
  return 0;

}
