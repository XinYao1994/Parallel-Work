#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#include "mpi.h"
#define BORD 100
#define CON 10000
#define NumPro 16
#define Ran (rand()%(BORD+1))

int bufa[NumPro], bufb[NumPro];

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
int Ycon, Fcon;

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
            *(chip + y*BORD + x) ++;
            if(y <= fun(x)) Ycon++;
        }
    }
    #pragma omp parallel for reduction(+:Fcon)
    for(i=0; i<=BORD; i++){
        for(j=0; j<=BORD; j++)
         Fcon += *(chip+i*BORD+j);
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
    //alloc a board
    int b = BORD + 1;
    chip = (int *)malloc(sizeof(int)*b*b);
    memset(chip, 0, sizeof(int)*b*b);
    Ycon = Fcon = 0;
    if(Pid==0){
        test(all/NumPro);
        int i;
        bufa[0] = Ycon;
        bufb[0] = Fcon;
        for(i=1; i<NumPro; i++){
            MPI_Recv(bufa+i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(bufb+i, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
        }
        int sum = 0;
        all = 0;
        #pragma omp parallel for reduction(+:sum, all)
        for(i=0; i<NumPro; i++){
            sum += bufa[i];
            all += bufb[i];
        } 
        printf("the area of fun from [0,%d] is %.2f",BORD,(double)(sum)/(double)(all)*(double)(BORD*BROD));
    }
    else{
        test(all/NumPro);
        MPI_Finalize();
        MPI_Send(&Ycon, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&Fcon, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        return 0;
    }
    MPI_Finalize();
    return 0;
}
