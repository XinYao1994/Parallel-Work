#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#include "mpi.h"
#define BORD 100
#define CON 10000
#define NumPro 16
#define Ran (rand()%(BORD+1))

int buf[NumPro];

int Myatoi(char *c){
    int sum = 0;
    while(*c != '\0'){
        sum *= 10;
        sum += (*c - '0');
        c++;
    }
    return sum;
}

int nPNum, Pid;
int Ycon;

int fun(int x){
    return x*x;
}

void test(int t){
    int i, j, x, y;
    #pragma omp parallel private(x, y)
    for(i=0; i<t; i++){
        x = Ran;
        y = Ran;
        #pragma omp critical
        {
            if(y <= fun(x)) Ycon++;
        }
    }
}

MPI_Status status;
int main(int argc, char **argv){
    int all;
    srand(time(0));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&Pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nPNum);
    if(argc == 2) all = Myatoi(*(argv+1)) * CON;
    else all = CON;
    //alloc a board
    int b = BORD + 1;
    Ycon = 0;
    if(Pid==0){
        test(all/NumPro);
        int i;
        buf[0] = Ycon;
        for(i=1; i<NumPro; i++){
            MPI_Recv(buf+i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        }
        int sum = 0;
        #pragma omp parallel for reduction(+:sum, all)
        for(i=0; i<NumPro; i++){
            sum += buf[i];
        } 
        printf("the area of fun from [0,%d] is %.2f",BORD,(double)(sum)/(double)(all)*(double)(BORD*BORD));
    }
    else{
        test(all/NumPro);
        MPI_Finalize();
        MPI_Send(&Ycon, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Finalize();
        return 0;
    }
    MPI_Finalize();
    return 0;
}
