
#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H


typedef struct {
  unsigned int width, height;
  float *r, *g, *b;
} imagem;

imagem abrir_imagem(char *nome_do_arquivo);
void salvar_imagem(char *nome_do_arquivo, imagem *I);
void liberar_imagem(imagem *i);
imagem brilho_v(imagem I, float b);
float maximo(imagem I);
imagem brilho_thread(imagem I, float fator, int tr);
imagem brilho_process(imagem I, float fator, int pr);
imagem brilho_h(imagem I, float fator);
#endif
