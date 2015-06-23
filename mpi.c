#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LA 1600
#define LB 1600
#define LC 1600
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
int xB, xE, yB, YE;
int *buf, *bufa, *bufb;

void init_Mm(int id, int x, int y, int same){
  int i,j,k;
  int all = Wp*Hp;
  xstep = x/all;
  ystep = y/all;
  xB = id * xstep;
  yB = id * ystep;
  xE = (id+1)*xstep;
  yE = (id+1)*ystep;
  bufa = (int *)malloc(sizeof(int)*ystep*same);
  bufb = (int *)malloc(sizeof(int)*xstep*same);
  int con = 0;
  for(i=yB;i<yE;i++)
     for(k=0;k<same;k++){
        *(a->m+i*a->b+k) = rand()%num;
        *(bufa+con) = *(a->m+i*a->b+k);
        con++;
     }
  con = 0;
  for(i=xB;i<xE;i++)
     for(k=0;k<same;k++){
        *(b->m+i*b->b+k) = rand()%num;
        *(bufb+con) = *(b->m+i*b->b+k);
        con++;
  }
}

//Send - oid
//Recive  - even
void init_con(int id, int same){
  int i,j;
  posy = id%Wp;//get group
  int bb = posy*Hp, ee = (posy+1)*Hp;
  int con;
  if(id%2){
    //send
    for(i=bb;i<ee;i++){
        if(i==id) continue;
        MPI_send(bufa, sizeof(bufa), MPI_INT, i, 1, MPI_COMM_WORLD);//send a
        MPI_send(bufb, sizeof(bufb), MPI_INT, i, 2, MPI_COMM_WORLD);//send b
    }
    //Recv
    for(i=bb;i<ee;i++){
        MPI_Recv(bufa, sizeof(bufa), MPI_INT, i, 1, MPI_COMM_WORLD, status);
        MPI_Recv(bufb, sizeof(bufb), MPI_INT, i, 2, MPI_COMM_WORLD, status);
        //fill in
        //xstep ystep not change
        con = 0;
        xB = i * xstep;
        yB = i * ystep;
        xE = (i+1)*xstep;
        yE = (i+1)*ystep;
        for(i=yB;i<yE;i++)
          for(k=0;k<same;k++){
            *(a->m+i*a->b+k) = *(bufa+con);
            con++;
          }
        con = 0;
        for(i=xB;i<xE;i++)
          for(k=0;k<same;k++){
            *(b->m+i*b->b+k) = *(bufb+con);
            con++;
          }
    }
  }
  else{
    //Recv
    int *buffa = (int *)malloc(sizeof(bufa));
    int *buffb = (int *)malloc(sizeof(bufb));
    for(i=bb;i<ee;i++){
        MPI_Recv(buffa, sizeof(buffa), MPI_INT, i, 1, MPI_COMM_WORLD, status);
        MPI_Recv(buffb, sizeof(buffb), MPI_INT, i, 2, MPI_COMM_WORLD, status);
        //fill in
        //xstep ystep not change
        con = 0;
        xB = i * xstep;
        yB = i * ystep;
        xE = (i+1)*xstep;
        yE = (i+1)*ystep;
        for(i=yB;i<yE;i++)
          for(k=0;k<same;k++){
            *(a->m+i*a->b+k) = *(buffa+con);
            con++;
          }
        con = 0;
        for(i=xB;i<xE;i++)
          for(k=0;k<same;k++){
            *(b->m+i*b->b+k) = *(buffb+con);
            con++;
          }
    }
    //send
    for(i=bb;i<ee;i++){
        if(i==id) continue;
        MPI_send(bufa, sizeof(bufa), MPI_INT, i, 1, MPI_COMM_WORLD);//send a
        MPI_send(bufb, sizeof(bufb), MPI_INT, i, 2, MPI_COMM_WORLD);//send b
    }
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
  xE = (posx+1)*xstep;
  yE = (posy+1)*ystep;
  //buf = (int *)malloc(sizeof(int)*xstep*ystep);
  if(id) MPI_Recv(buf, sizeof(int)*xstep*ystep, MPI_INT, id, 0, MPI_COMM_WORLD, status);
  int con = 0;
  for(i=xB;i<xE;i++)
    for(j=yB;j<yE;j++){
       *(c->m+i*x+j) = *(buf+con);
       con++;
    }
}

void count(int id, int over){
  int i,j,k,con = 0;
  posx = id%Wp;
  posy = id/Hp;
  xstep = x/Wp;
  ystep = y/Hp;
  xB = posx * xstep;
  yB = posy * ystep;
  xE = (posx+1)*xstep;
  yE = (posy+1)*ystep;
  buf = (int *)malloc(xstep*ystep*sizeof(int));//
  for(i=yB;i<yE;i++)
     for(j=xB;j<xE;j++){
         for(k=0;k<over;k++)
             *(buf+con) += (*(a->m+i*a->a+k)) * (*(b->m+k*b->a+j));
         con++;
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
    printf("use 1600, 1600, 1600\n");
    la = LA; lb = LB; lc = LC;
  }
  else{
    if(argc>1) printf("argc error,use 1600, 1600, 1600\n");
    la = LA; lb = LB; lc = LC;
  }
  if(la%(Wp*Hp)!=0||lc%(Wp*Hp)!=0) {
    printf("adjust the Wp, Hp or change the input");
  }
  //alloc the matrix
  allocMm(&a, la, lb);
  allocMm(&b, lb, lc);
  if(Pid==0){
    init_Mm(Pid, lc, la, lb);
    init_con(Pid);
    count(Pid, lb);
    allocMm(&c, la, lc);
    for(int i=0;i<nPNum;i++){
        fill(i, lc, la, status);
    }
  }
  else{
    init_Mm(Pid, lc, la, lb);
    init_con(Pid);
    count(Pid, lb);
    MPI_send(buf, xlen*ylen*sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  MPI_Finalize();
  return 0;
}
