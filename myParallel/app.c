#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include "func.h"
#include "parallel.h"
void run_worker_rank();

int main(int argc, char** argv)
{
	int rank, numProcs;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	
	if(numProcs != 2)
	{
		printf("The program should run with two processes only\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return 0;

	}
	
	if(rank == MASTER)
	{
		float calc_mathcing = 0.0;
		Picture* pictures = NULL;
		Object* objects = NULL;
		int numPictures = 0, numOfObj = 0;
		double start = 0.0, end = 0.0;
		double execTime = 0.0;
		
		
		int numPicturesForWorker = 0, numPicturesForMaster = 0;
		Picture* picturesForWorker = NULL;
		Picture* picturesForMaster = NULL;
		
		
		
	    printf("--------parallel program--------\n");
		if (!readInputFile(&pictures, &numPictures, &objects, &numOfObj, &calc_mathcing)) // reading input file
		{
			printf("Failed to read from Input file\n");
			return 0;
		}
		
        // input.txt content 
		int numPicturesPerRank = numPictures / numProcs; // 2
		int remainingPicture = numPictures % numProcs; // 1

		numPicturesForMaster = numPicturesPerRank * (numProcs - 1) + remainingPicture;; // 3
		numPicturesForWorker = numPictures - numPicturesForMaster; // 2
		


		// allocate for worker and master
		picturesForWorker = (Picture*)malloc(numPicturesForWorker * sizeof(Picture));
		picturesForMaster = (Picture*)malloc(numPicturesForMaster * sizeof(Picture));


		int j = 0;
		int k = 0;
		for(int i = 0; i < numPictures; i++)
		{
			
			for (int j = 0; j < DIFF_OBJ_REQ; j++) // init -1 in all foundObj and positions
			{
				pictures[i].foundObj[j] = -1;
				for (int k = 0; k < 2; k++)
				{
					pictures[i].positions[j][k] = -1;
				}
			}	
			
			if (j < numPicturesForMaster)
			{
				picturesForMaster[j] = pictures[i];
				j++;
			}
			else
			{
				picturesForWorker[k] = pictures[i];
				k++;
			}
			
		}
		
		
		
		printf("Searching pictures for objects:\n");
		// find the matching for each object in the picture
		start = MPI_Wtime();
		
		printf("Searching in rank 0\n");
		#pragma omp parallel for num_threads(NUM_THREADS)
		for(int i = 0; i < numPicturesForMaster; i++)
		{			
			findMatching(&picturesForMaster[i], objects, numOfObj, calc_mathcing, MASTER);
		}
			
		
		
		printf("---------------------\n");
		printf("Passing data from rank 0  to rank 1:\n");	
		
		MPI_Send(&numPicturesForWorker, 1, MPI_INT, WORKER, 0, MPI_COMM_WORLD);
		MPI_Send(&numOfObj, 1, MPI_INT, WORKER, 0, MPI_COMM_WORLD);
		MPI_Send(&calc_mathcing, 1, MPI_FLOAT, WORKER, 0, MPI_COMM_WORLD);
		
		
		printf("---------------------\n");
		printf("Sending %d pictures and %d objects from rank 0 to rank 1:\n", numPicturesForWorker, numOfObj);
		
		MPI_SendObjectsToWorker(objects, numOfObj);
		MPI_SendPicturesToWorker(picturesForWorker, numPicturesForWorker);// send all data 
		end = MPI_Wtime();
		
		MPI_RecvResultsFromWorker(picturesForWorker, numPicturesForWorker);
		printf("Received Results from rank 1 to rank 0 for %d number of pictures\n", numPicturesForWorker);
		
		printf("---------------------\n");
		printf("Merge pictures to one struct\n");
		mergePic(pictures, picturesForMaster, picturesForWorker, numPictures, numPicturesForWorker, numPicturesForMaster);
		if (!writeToOutputFile(pictures, numPictures))
		{
			printf("Failed to write to output file\n");
			return 0;
		}
		execTime = end - start;
		printf("---------------------\n");
		printf("Exec time of parallel program: %f sec\n", execTime);
		printf("---------------------\n");
			
			
		
			
		freeAllocations(pictures, numPictures, objects, numOfObj);

		free(picturesForWorker);
		free(picturesForMaster);
		printf("finished last free allocation rank 0\n");	
		printf("---------------------\n");
		
	}
	else if(rank == WORKER)
	{
		float* calc_mathcing = (float*)malloc(sizeof(float));
		int* numPicturesForWorker = (int*)malloc(sizeof(int));; 
		int* numOfObj = (int*)malloc(sizeof(int));;
		
		
		MPI_Recv(numPicturesForWorker, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		MPI_Recv(numOfObj, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		MPI_Recv(calc_mathcing, 1, MPI_FLOAT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		
		printf("---------------------\n");
		
		Object* objects = MPI_RecvObjectsFromMaster(*numOfObj);
		Picture* picturesForWorker = MPI_RecvPicturesFromMaster(*numPicturesForWorker); 
		
		printf("Received %d objects and %d pictures and also matching val %f that rank 1 will use form rank 0\n", *numOfObj, *numPicturesForWorker, *calc_mathcing);
		
		printf("---------------------\n");
		
		
		printf("---------------------\n");
		printf("Searching in rank 1\n");
		#pragma omp parallel for num_threads(NUM_THREADS)
		for(int i = 0; i < *numPicturesForWorker; i++)
		{			
			findMatching(&picturesForWorker[i], objects, *numOfObj, *calc_mathcing, WORKER);
		}
		printf("---------------------\n");
		
		printf("Sending results from rank 1 to rank 0\n");
		
		MPI_SendResultsToMaster(picturesForWorker, *numPicturesForWorker);
		
		printf("Finished Sending results from rank 1 to rank 0\n");
		printf("---------------------\n");
		freeAllocations(picturesForWorker, *numPicturesForWorker, objects, *numOfObj);
		free(numOfObj);
		free(numPicturesForWorker);
		free(calc_mathcing);
		printf("finished free allocation rank 1\n");
		printf("---------------------\n");
	}	
	MPI_Finalize();
	return 0;
}





