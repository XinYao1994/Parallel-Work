#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LA 1000
#define LB 1000
#define LC 1000
#define num 50
//use 4*4 = 16 pro
#define Wp 4
#define Hp 4

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

void allocMm(Matrix **m, int x,int y){
  *m = (Matrix *)malloc(sizeof(Matrix));
  (*m)->a = x;
  (*m)->b = y;
  (*m)->m = (int *)malloc(sizeof(int)*x*y);
}

int posx, posy, xstep, ystep;
int xB, xE, yB, YE, xlen, ylen;
int *buf;

void init_Mm(int id, int x, int y){
  int i,j;
  posx = id%Wp;
  posy = id/Hp;
  xstep = x/Wp;
  ystep = y/Hp;
  xB = posx * xstep;
  yB = posy * ystep;
  if(posx == (Wp-1)) xE = x;
  else xE = (posx+1)*xstep;
  if(posy == (Hp-1)) yE = y;
  else yE = (posy+1)*ystep;
  xlen = xE-xB;
  ylen = yE-yB;
  buf = (int *)malloc(sizeof(int)*xlen*ylen);
  memset(buf, 0, sizeof(buf));
  for(i=xB;i<xE;i++)
    for(j=yB;j<yE;j++){
       *(a->m+i*x+j) = rand()%num;
       *(b->m+i*x+j) = rand()%num;
    }
}

void Fill(int id, int x, int y, MPI_Status *status){
  int i,j;
  posx = id%Wp;
  posy = id/Hp;
  xstep = x/Wp;
  ystep = y/Hp;
  xB = posx * xstep;
  yB = posy * ystep;
  if(posx == (Wp-1)) xE = x;
  else xE = (posx+1)*xstep;
  if(posy == (Hp-1)) yE = y;
  else yE = (posy+1)*ystep;
  xlen = xE-xB;
  ylen = yE-yB;
  buf = (int *)malloc(sizeof(int)*xlen*ylen);
  MPI_Recv(buf, sizeof(int)*xlen*ylen, MPI_INT, id, 0, MPI_COMM_WORLD, status);
  int count = 0;
  for(i=xB;i<xE;i++)
    for(j=yB;j<yE;j++){
       *(c->m+i*x+j) = *(buf+count);
       count++;
    }
}

void count(int over){
  int i,j,k,count = 0;
  for(i=yB;i<yE;i++)
     for(j=xB;j<xE;j++){
         for(k=0;k<over;k++)
             *(buf+count) += (*(a->m+i*a->a+k)) * (*(b->m+k*b->a+j));
         count++;
     }    
}

int nPNum, Pid;

int main(int argc, char **argv){
  srand(time(0));
  int la, lb, lc;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&Pid);
  MPI_Comm_size(MPI_COMM_WORLD,&nPNum);
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
  allocMm(&a, la, lb);
  allocMm(&b, lb, lc);
  //allocMm(&c, la, lc);
  if(Pid==0){
    init_Mm(Pid, lc, la);
    allocMm(&c, la, lc);
    count(lb);
    for(int i=1;i<nPNum;i++){
        fill(i, lc, la, status);
    }
  }
  else{
    init_Mm(Pid, lc, la);
    count(lb);
    MPI_send(buf, xlen*ylen*sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  MPI_Finalize();
  return 0;
}
