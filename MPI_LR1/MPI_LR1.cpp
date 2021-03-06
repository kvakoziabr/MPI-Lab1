// MPI_LR1.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "windows.h"
#include "mpi.h"
#include <ctime>
#include <cmath>

int const Iteration = 10000;
const const int MASS_SIZE = 250;

void Capacity()
{
	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int SendData[MASS_SIZE];
	int RecvData[MASS_SIZE];
	double StartTime = MPI_Wtime();


	for (int i = 0;  i <= Iteration; i++) {
		if (rank == 0)
		{
			MPI_Send(SendData, MASS_SIZE, MPI_INT, size-1, 1, MPI_COMM_WORLD);
		//	MPI_Recv(RecvData, MASS_SIZE, MPI_INT, size-1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			if (i == Iteration)
			{
				printf("Capacity Time is %f for %d, Size %d \n", MPI_Wtime() - StartTime, Iteration, sizeof(MPI_INT)*MASS_SIZE);
			}
		}

		if (rank == size-1)
		{
			MPI_Recv(RecvData, MASS_SIZE, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//	MPI_Send(SendData, MASS_SIZE, MPI_INT, 0, 2, MPI_COMM_WORLD);
		}
	}
}

void CheckLatency()
{
	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int RecvData;
	double StartTime = MPI_Wtime();

	for (int i = 0; i <= Iteration / 2; i++) {
		if (rank == 0)
		{
			MPI_Send(NULL, 0, MPI_INT, size-1, 1, MPI_COMM_WORLD);
			MPI_Recv(NULL, 0, MPI_INT, size-1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (i == Iteration / 2)
			{
				printf("Check Latency Time is %f for %d \n", MPI_Wtime() - StartTime, Iteration);
			}
		}

		if (rank == size-1)
		{
			MPI_Recv(NULL, 0, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(NULL, 0, MPI_INT, 0, 2, MPI_COMM_WORLD);
		}
	}
}

void BCastFake()
{
	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int SendData[MASS_SIZE];
	int RecvData[MASS_SIZE];
	double StartTime = MPI_Wtime();


	for (int i = 0; i <= Iteration; i++) {
		if (rank == 0)
		{
			for (int j = 1; j < size; j++)
				MPI_Send(SendData, MASS_SIZE, MPI_INT, j, 1, MPI_COMM_WORLD);
		}

		else
		{
			MPI_Recv(RecvData, MASS_SIZE, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	if (rank ==  1)
	{
		printf("Fake Bcast Time is %f for %d, Size %d \n", MPI_Wtime() - StartTime, Iteration, sizeof(MPI_INT)*MASS_SIZE);
	}
}

void BCast()
{
	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int SendData[MASS_SIZE];
	int RecvData[MASS_SIZE];
	int StartTime = MPI_Wtime();

	for (int i = 0; i <= Iteration; i++)
	{
		MPI_Bcast(SendData, MASS_SIZE, MPI_INT, 1, MPI_COMM_WORLD);
	}

	if (rank == size-1)
	{
		printf("Bcast Real Time is %f for %d, Size %d \n", MPI_Wtime() - StartTime, Iteration, sizeof(MPI_INT)*MASS_SIZE);
	}
}

void BCastTest()
{
	BCast();
	BCastFake();
}

void Reduce()
{
	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int SendData[MASS_SIZE];
	int RecvData[MASS_SIZE];
	int StartTime;


	if (rank == 0) {
		StartTime = MPI_Wtime();
	}

	for (int i = 0; i < Iteration; i++)
	{
		MPI_Reduce(SendData, RecvData, MASS_SIZE, MPI_INT, MPI_SUM, 0,MPI_COMM_WORLD);
	}

	if (rank == 0)
		printf("Real Reduce Time is %f for  %d  Size %d \n", MPI_Wtime() - StartTime, Iteration, sizeof(MPI_INT)*MASS_SIZE);
}

void FakeReduce()
{
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int SendData[MASS_SIZE];
	int RecvData[MASS_SIZE];
	int r[MASS_SIZE];
	int StartTime;

	for (int i = 0; i < MASS_SIZE; i++) {
		SendData[i] = pow(2, sizeof(MPI_INT) - 1);
	}

	if (rank == 0) {
		StartTime = MPI_Wtime();
	}
	for (int iter = 0; iter <= Iteration; iter++) {
		if (rank == 0)
		{
			for (int i = 1; i < size; i++)
			{				
				MPI_Recv(r, MASS_SIZE, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				for (int j = 0; j < MASS_SIZE; j++)
				{
					RecvData[j] += r[j];
				}
			}
		}
		else
		{
			MPI_Send(SendData, MASS_SIZE, MPI_INT, 0, 1, MPI_COMM_WORLD);
		}
	}

	if (rank == 0)
		printf("Fake Reduce Time is %f for  %d  Size %d \n", MPI_Wtime() - StartTime, Iteration, sizeof(MPI_INT)*MASS_SIZE);
}

void ReduceTest()
{
	FakeReduce();
	Reduce();
}

int main(int argc, char * argv[])
{	
	MPI_Init(&argc, &argv);

	CheckLatency();
	Capacity();
	BCastTest();
	ReduceTest();

	MPI_Finalize();
	return 0;
}