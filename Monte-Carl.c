#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#include "mpi.h"
#define BORD 1000
#define CON 100000
#define NumPro 16
int Myatoi(char *c){
    int sum = 0;
    while(*c != '\0'){
        sum *= 10;
        sum += (*c - '0');
        c++;
    }
    return sum;
}

int *chip;
int nPNum, Pid;

int main(int argc, char **argv){
    int all;
    srand(time(0));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&Pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nPNum);
    if(argc == 2) con = Myatoi(*(argv+1));
    //alloc a board
    chip = (int *)malloc(sizeof(int)*BORD*BORD);
    return 0;
}
