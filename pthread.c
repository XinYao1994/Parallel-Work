#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define LA 1000
#define LB 1000
#define LC 1000
#define num 50
#define diva 3
#define divc 3
//we should use only 9 threads at most to run it

typedef struct{
  int *m;
  int a,b;
}Matrix;

typedef struct{
  int x,y;
  int xlen, ylen;
}Point;

Matrix *a,*b,*c;
pthread_t hThread[diva][divc];
Point array[diva][divc];

int Myatoi(char *c){
  int sum = 0;
  while(*c != '\0'){
    sum *= 10;
    sum += (*c - '0');
    c++;
  }
  return sum;
}

//(ax, ax+alen)*(cy, cy+clen) with b
void *MultMatrix(void *arg){
  int i, j, k;
  int ax, alen, cy, clen; 
  Point *p = (Point *)arg;
  ax = p->x;
  alen = p->xlen;
  cy = p->y;
  clen = p->ylen;
  for(i=ax; i<(ax+alen); i++)
     for(j=cy; j<(cy+clen); j++)
        for(k=0; k<a->b; k++)
           *(c->m+i*c->b+j) += (*(a->m+i*a->b+k))*(*(b->m+k*b->b+j));
}

void allocMm(Matrix **m, int x,int y,int init){
  *m = (Matrix *)malloc(sizeof(Matrix));
  (*m)->a = x;
  (*m)->b = y;
  (*m)->m = (int *)malloc(sizeof(int)*x*y);
  if(!init){
     memset((*m)->m,0,sizeof(int)*x*y);
     return ;
  }
  int i,j;
  for(i=0;i<x;i++)
    for(j=0;j<y;j++)
       *((*m)->m+i*y+j) = rand()%num;
}

int main(int argc, char **argv){
  srand(time(0));
  int la, lb, lc;
  if(argc==2){
    la = lb = lc = atoi(*(argv+1));
  }
  else if(argc==4){
    la = Myatoi(*(argv+1));
    lb = Myatoi(*(argv+2));
    lc = Myatoi(*(argv+3));
  }
  else if(argc==1){
    printf("use 1000, 1000, 1000\n");
    la = LA; lb = LB; lc = LC;
  }
  else{
    if(argc>1) printf("argc error,use 1000, 1000, 1000\n");
    la = LA; lb = LB; lc = LC;
  }
  //alloc the matrix
  allocMm(&a, la, lb, 1);
  allocMm(&b, lb, lc, 1);
  allocMm(&c, la, lc, 0);
  int i,j;
  int adis = la/diva;
  int cdis = lc/divc;
  for(i=0; i<diva; i++)
     for(j=0; j<divc; j++){//x, y. xlen, ylen
        array[i][j].x = i*adis;
        array[i][j].y = i*cdis;
        if(i+1==diva) array[i][j].xlen = la - i*adis;
        else array[i][j].xlen = adis;
        if(j+1==divc) array[i][j].ylen = lc - i*cdis;
        else array[i][j].ylen = cdis;
        if(pthread_create(&hThread[i][j], NULL, (void *)MultMatrix, (void *)&array[i][j])){
           printf("create error!\n");
           return 0;
        }
     }
  for(i=0; i<diva; i++) 
     for(j=0; j<divc; j++)
        pthread_join(hThread[i][j], NULL); 
  return 0;
}
