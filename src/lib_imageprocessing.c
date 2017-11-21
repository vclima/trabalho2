
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#include "imageprocessing.h"

#include <FreeImage.h>

/*
imagem abrir_imagem(char *nome_do_arquivo);
void salvar_imagem(char *nome_do_arquivo);
void liberar_imagem(imagem *i);
 */
typedef struct args {
  imagem* I;
  int thread;
  int idxstart;
  int idxfinal;
  float fator;
} args;

void* funcao_thread (void *arg){
  args* limite = (args*) arg;
  //printf("Thread %d pixel %d a %d\n",limite->thread,limite->idxstart,limite->idxfinal);
  for (int idx=limite->idxstart; idx<limite->idxfinal; idx++) {
       limite->I->r[idx] = limite->I->r[idx]*limite->fator;
       if(limite->I->r[idx]>255){
         limite->I->r[idx]=255;
       }
       limite->I->g[idx] = limite->I->g[idx]*limite->fator;
       if(limite->I->g[idx]>255){
         limite->I->g[idx]=255;
       }
       limite->I->b[idx] = limite->I->b[idx]*limite->fator;
       if(limite->I->b[idx]>255){
         limite->I->b[idx]=255;
       }
     }
   return NULL;
}
imagem abrir_imagem(char *nome_do_arquivo) {
  FIBITMAP *bitmapIn;
  int x, y;
  RGBQUAD color;
  imagem I;

  bitmapIn = FreeImage_Load(FIF_JPEG, nome_do_arquivo, 0);

  if (bitmapIn == 0) {
    printf("Erro! Nao achei arquivo - %s\n", nome_do_arquivo);
  } else {
    printf("Arquivo lido corretamente!\n");
   }

  x = FreeImage_GetWidth(bitmapIn);
  y = FreeImage_GetHeight(bitmapIn);

  I.width = x;
  I.height = y;
  I.r = malloc(sizeof(float) * x * y);
  I.g = malloc(sizeof(float) * x * y);
  I.b = malloc(sizeof(float) * x * y);

   for (int i=0; i<x; i++) {
     for (int j=0; j <y; j++) {
      int idx;
      FreeImage_GetPixelColor(bitmapIn, i, j, &color);

      idx = i + (j*x);

      I.r[idx] = color.rgbRed;
      I.g[idx] = color.rgbGreen;
      I.b[idx] = color.rgbBlue;
    }
   }
  return I;

}
void liberar_imagem(imagem *I) {
  free(I->r);
  free(I->g);
  free(I->b);
}
void salvar_imagem(char *nome_do_arquivo, imagem *I) {
  FIBITMAP *bitmapOut;
  RGBQUAD color;

  printf("Salvando imagem %d por %d...\n", I->width, I->height);
  bitmapOut = FreeImage_Allocate(I->width, I->height, 24, 0, 0, 0);

   for (int i=0; i<I->width; i++) {
     for (int j=0; j<I->height; j++) {
      int idx;

      idx = i + (j*I->width);
      color.rgbRed = I->r[idx];
      color.rgbGreen = I->g[idx];
      color.rgbBlue = I->b[idx];

      FreeImage_SetPixelColor(bitmapOut, i, j, &color);
    }
  }

  FreeImage_Save(FIF_JPEG, bitmapOut, nome_do_arquivo, JPEG_DEFAULT);
}
imagem brilho_v(imagem I, float fator){
  clock_t ct0, ct1, dct; /* Medida de tempo baseada no clock da CPU */
  struct timeval rt0, rt1, drt; /* Tempo baseada em tempo real */
  gettimeofday(&rt0, NULL);
  ct0 = clock();
  for (int i=0; i<I.width; i++) {
    for (int j=0; j<I.height; j++) {
     int idx;

     idx = i + (j*I.width);
     I.r[idx] = I.r[idx]*fator;
     if(I.r[idx]>255){
       I.r[idx]=255;
     }
     I.g[idx] = I.g[idx]*fator;
     if(I.g[idx]>255){
       I.g[idx]=255;
     }
     I.b[idx] = I.b[idx]*fator;
     if(I.b[idx]>255){
       I.b[idx]=255;
     }
   }
 }
 ct1 = clock();
 gettimeofday(&rt1, NULL);
 timersub(&rt1, &rt0, &drt);
 printf("\nVarredura vertical\nTempo real: %ld.%06ld segundos\n", drt.tv_sec, drt.tv_usec);
 printf("Tempo user: %f segundos\n", (double)(ct1-ct0)/CLOCKS_PER_SEC);
 return I;
}
imagem brilho_h(imagem I, float fator){
  clock_t ct0, ct1, dct; /* Medida de tempo baseada no clock da CPU */
  struct timeval rt0, rt1, drt; /* Tempo baseada em tempo real */
  gettimeofday(&rt0, NULL);
  ct0 = clock();
  int total=I.width*I.height;
  for (int idx=0; idx<total; idx++) {
    I.r[idx] = I.r[idx]*fator;
    if(I.r[idx]>255){
     I.r[idx]=255;
    }
    I.g[idx] = I.g[idx]*fator;
    if(I.g[idx]>255){
     I.g[idx]=255;
    }
    I.b[idx] = I.b[idx]*fator;
    if(I.b[idx]>255){
     I.b[idx]=255;
    }
 }
 ct1 = clock();
 gettimeofday(&rt1, NULL);
 timersub(&rt1, &rt0, &drt);
 printf("\nVarredura horizontal\nTempo real: %ld.%06ld segundos\n", drt.tv_sec, drt.tv_usec);
 printf("Tempo user: %f segundos\n", (double)(ct1-ct0)/CLOCKS_PER_SEC);
 return I;
}
imagem brilho_thread(imagem I, float fator, int tr){
  clock_t ct0, ct1, dct; /* Medida de tempo baseada no clock da CPU */
  struct timeval rt0, rt1, drt; /* Tempo baseada em tempo real */
  gettimeofday(&rt0, NULL);
  ct0 = clock();
  pthread_t threads[tr];
  void *arg;
  args argumento[tr];
  int i, passo;
  int total=I.width*I.height;
  passo=total/tr;
  for(i=0;i<(tr-1);i++){
    argumento[i].I=&I;
    argumento[i].thread=i+1;
    argumento[i].fator=fator;
    argumento[i].idxstart=passo*i;
    argumento[i].idxfinal=argumento[i].idxstart+passo-1;
    arg=(void*)&(argumento[i]);
    pthread_create(&(threads[i]),NULL,funcao_thread,arg);
  }
  argumento[i].I=&I;
  argumento[i].thread=i+1;
  argumento[i].fator=fator;
  argumento[i].idxstart=passo*i;
  argumento[i].idxfinal=total;
  arg=(void*)&argumento[i];
  pthread_create(&(threads[i]),NULL,funcao_thread,arg);
  for(i=0;i<tr;i++){
    pthread_join(threads[i],NULL);
  }
  ct1 = clock();
  gettimeofday(&rt1, NULL);
  timersub(&rt1, &rt0, &drt);
  printf("\nVarredura horizontal por threads\n# threads:%d\n",tr);
  printf("Tempo real: %ld.%06ld segundos\n", drt.tv_sec, drt.tv_usec);
  printf("Tempo user: %f segundos\n", (double)(ct1-ct0)/CLOCKS_PER_SEC);
  return I;
}
float maximo(imagem I){
  float norma,max;
  max=0;
  norma=0;
  for (int i=0; i<I.width; i++) {
    for (int j=0; j<I.height; j++) {
     int idx;
     idx = i + (j*I.width);
     norma=sqrt(I.r[idx]*I.r[idx]+I.g[idx]*I.g[idx]+I.b[idx]*I.b[idx]);
     if(norma>max){
       max=norma;
     }
   }
 }
 return max;
}
imagem brilho_process(imagem I, float fator, int pr){
  clock_t ct0, ct1, dct; /* Medida de tempo baseada no clock da CPU */
  struct timeval rt0, rt1, drt; /* Tempo baseada em tempo real */
  gettimeofday(&rt0, NULL);
  ct0 = clock();
  int protection=PROT_READ|PROT_WRITE;
  int visibility=MAP_SHARED|MAP_ANON;
  pid_t filho[pr];
  float *r,*g,*b,*f;
  int i,*idxs,*idxf, passo, total;
  total=I.width*I.height;
  f=(float*)mmap(NULL,sizeof(float),protection,visibility,0,0);
  r=(float*)mmap(NULL,(sizeof(float)*total),protection,visibility,0,0);
  g=(float*)mmap(NULL,(sizeof(float)*total),protection,visibility,0,0);
  b=(float*)mmap(NULL,(sizeof(float)*total),protection,visibility,0,0);
  *f=fator;
  for(i=0;i<total;i++){
    r[i]=I.r[i];
    b[i]=I.b[i];
    g[i]=I.g[i];
  }
  idxs=(int*)mmap(NULL,(sizeof(int)*pr),protection,visibility,0,0);
  idxf=(int*)mmap(NULL,(sizeof(int)*pr),protection,visibility,0,0);
  passo=(total)/pr;
  for(i=0;i<(pr-1);i++){
    idxs[i]=passo*i;
    idxf[i]=idxs[i]+passo-1;
  }
  idxs[i]=passo*i;
  idxf[i]=total;
  for(i=0;i<pr;i++){
    filho[i]=fork();
    if (filho[i]==0){
      for(int j=idxs[i];j<idxf[i];j++){
        r[j]=r[j]*(*f);
        if(r[j]>255){
          r[j]=255;
        }
        b[j]=b[j]*(*f);
        if(b[j]>255){
          b[j]=255;
        }
        g[j]=g[j]*(*f);
        if(g[j]>255){
          g[j]=255;
        }
      }
      exit(0);
    }
  }
  for(i=0;i<pr;i++){
    waitpid(filho[i],NULL,0);
  }
  for(i=0;i<total;i++){
    I.r[i]=r[i];
    I.b[i]=b[i];
    I.g[i]=g[i];
  }
  ct1 = clock();
  gettimeofday(&rt1, NULL);
  timersub(&rt1, &rt0, &drt);
  printf("\nVarredura horizontal por processos\n# processos:%d\n",pr);
  printf("Tempo real: %ld.%06ld segundos\n", drt.tv_sec, drt.tv_usec);
  printf("Tempo user: %f segundos\n", (double)(ct1-ct0)/CLOCKS_PER_SEC);
  return I;
}
