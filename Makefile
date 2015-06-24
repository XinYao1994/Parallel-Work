#compiler
CC = gcc
MPICC = mpicc
MPIRUN = mpirun
CUDACC = nvcc
CFLAGS = -g -O3 
#begin to choose


pthread: pthread.c
	$(CC) -o pthread pthread.c -lpthread $(CFLAGS)

openmp: openmp.c
	$(CC) -o openmp openmp.c -fopenmp $(CFLAGS)

mpi: mpi.c
	$(MPICC) -o mpi mpi.c $(CFLAGS)
	#$(MPIRUN) -np 16 ./mpi

cuda: cuda.cu
	$(CUDACC) -o cuda cuda.cu -lcudart $(CFLAGS) -G
	$(CUDACC) -run ./cuda

mc: Monte-Carl.c
	$(MPICC) -o mc Monte-Carl.c -fopenmp $(CFLAGS)

clean:
	-rm -f pthread openmp mpi cuda
