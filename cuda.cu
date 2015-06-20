#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cuda_runtime.h>
//#pragma comment(lib, "cudart.lib")
#define LA 1024
#define LB 1024
#define LC 1024
#define num 50

typedef struct{
  int *m;
  int a,b;
}Matrix;

cudaError_t error;
int block_size = 16;

bool init(){
  int count;
  int devID = 0;
  cudaDeviceProp deviceProp;
  cudaGetDeviceCount(&count);
  if(count == 0){
     printf("no device, exit\n");
     return false;
  }
  error = cudaGetDevice(&devID);
  error = cudaGetDeviceProperties(&deviceProp, devID);
  if(error != cudaSuccess){
     printf("device not perpared,exit\n");
     return false;
  }
  block_size = (deviceProp.major < 2) ? 16 : 32;
  return true;
}

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

template <int BLOCK_SIZE> __global__ void
matrixMulCuda(int *c, int *a, int *b, int wA, int wB){
  int bx = blockIdx.x;
  int by = blockIdx.y;
  int tx = threadIdx.x;
  int ty = threadIdx.y;
  
  int aBegin = wA * BLOCK_SIZE * by;
  int aEnd = aBegin + wA - 1;
  int aStep  = BLOCK_SIZE;
  int bBegin = BLOCK_SIZE * bx;
  int bStep  = BLOCK_SIZE * wB;
  int Cadd = 0;
  for (int i = aBegin, j = bBegin;i <= aEnd; i += aStep, j += bStep)
  {
      __shared__ float As[BLOCK_SIZE][BLOCK_SIZE];
      __shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE];
      As[ty][tx] = a[i + wA * ty + tx];
      Bs[ty][tx] = b[j + wB * ty + tx];
      __syncthreads();
      for (int k = 0; k < BLOCK_SIZE; ++k)
      {
            Cadd += As[ty][k] * Bs[k][tx];
      }
      __syncthreads();
    }
    int w = wB * BLOCK_SIZE * by + BLOCK_SIZE * bx;
    c[w + wB * ty + tx] = Cadd;
}

void matrixMul(Matrix *a, Matrix *b, Matrix *c, int block_size){
  int *d_a, *d_b, *d_c;
  cudaMalloc((void **)&d_a, sizeof(int)*a->a*a->b);
  cudaMalloc((void **)&d_b, sizeof(int)*b->a*b->b);
  cudaMalloc((void **)&d_c, sizeof(int)*c->a*c->b);
  error = cudaMemcpy(d_a, a->m, sizeof(int)*a->a*a->b, cudaMemcpyHostToDevice);
  error = cudaMemcpy(d_b, b->m, sizeof(int)*b->a*b->b, cudaMemcpyHostToDevice);
  if(error != cudaSuccess) {
     printf("copy failed from host to device, exit\n");
     exit(0);
  }
  dim3 threads(block_size, block_size);
  dim3 grid(b->b/threads.x, a->a/threads.y);
  if(block_size==16) 
     matrixMulCuda<16><<< grid, threads >>>(d_c, d_a, d_b, a->b, b->b);
  else
     matrixMulCuda<32><<< grid, threads >>>(d_c, d_a, d_b, a->b, b->b);
  error = cudaMemcpy(c->m, d_c, sizeof(int)*c->a*c->b, cudaMemcpyDeviceToHost);
  if(error != cudaSuccess)  {
     printf("copy failed from device to host, exit\n");
     exit(0);
  }
  cudaFree(d_a);
  cudaFree(d_b);
  cudaFree(d_c);
}

int main(int argc, char **argv){
  srand(time(0));
  if(!init()) exit(0);
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
    printf("use 1024, 1024, 1024\n");
    la = LA; lb = LB; lc = LC;
  }
  else{
    if(argc>1) printf("argc error,use 1024, 1024, 1024\n");
    la = LA; lb = LB; lc = LC;
  }
  if(!la%block_size || !lb%block_size || !lc%block_size){
    printf("set matrix's parameter%(%d) = 0\n",block_size);
    return;
  }
  //alloc the matrix
  allocMm(&a, la, lb, 1);
  allocMm(&b, lb, lc, 1);
  allocMm(&c, la, lc, 0);    
  matrixMul(a, b, c, block_size);
  return 0;
}
