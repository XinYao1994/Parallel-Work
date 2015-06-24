#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#include "mpi.h"
#define BORD 100
#define CON 10000
#define NumPro 16

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
int yBord;
int fun(int x){
    return x*x;
}

void test(int t){
    int i, x, y;
    yBord = fun(BORD);
    Ycon = 0;
    #pragma omp parallel for
    for(i=0; i<t; i++){
        x = rand()%(BORD+1);
        y = rand()%(yBord+1);
        if(y <= fun(x)){
            #pragma omp critical
            Ycon = Ycon + 1;
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
    if(Pid==0){
        test(all/NumPro);
        int i;
        buf[0] = Ycon;
        for(i=1; i<NumPro; i++){
            MPI_Recv(buf+i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        }
        int sum = 0;
        #pragma omp parallel for reduction(+:sum)
        for(i=0; i<NumPro; i++){
            sum += buf[i];
        } 
        printf("the area of fun from [0,%d] is %.2f\n",BORD,(double)(sum)/(double)(all)*(double)(BORD*yBord));
    }
    else{
        test(all/NumPro);
        MPI_Send(&Ycon, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Finalize();
        return 0;
    }
    MPI_Finalize();
    return 0;
}
