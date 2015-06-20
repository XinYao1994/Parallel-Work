#include <stdio.h>
#include "omp.h"
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

int Myatoi(char *c){
  int sum = 0;
  while(*c != '\0'){
    sum *= 10;
    sum += (*c - '0');
    c++;
  }
  return sum;
}

Matrix *a,*b,*c;

void openmp_multmatrix(){
  int i,j,k;
  int index;
  int all = a->a * b->b;
  #pragma omp parallel private(i,j,k)
  for(index=0; index<all; index++){
     i = index/b->b;
     j = index%b->b;
     for(k=0; k<a->b; k++){
        *(c->m+i*c->b+j) += (*(a->m+i*a->b+k))*(*(b->m+k*b->b+j));
     }
  } 
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
  openmp_multmatrix();
  return 0;
}
