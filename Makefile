#compiler
CC = gcc
MPICC = mpicc
CUDACC = nvcc
CFLAGS = -g -O3 
#begin to choose


pthread: pthread.c
	$(CC) -o pthread pthread.c -lpthread $(CFLAGS)

openmp: openmp.c
	$(CC) -o openmp openmp.c -fopenmp $(CFLAGS)

mpi: mpi.c
	$(MPICC) -o mpi mpi.c $(CFLAGS)

cuda: cuda.cu
  $(CUDACC) 

clean:
	-rm -f pthread openmp mpi cuda
