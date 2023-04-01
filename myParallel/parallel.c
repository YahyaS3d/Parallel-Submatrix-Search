#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "parallel.h"



void MPI_SendObjectsToWorker(Object* objects, int numObjects)
{
	int* Array1D = (int*)malloc(MAX_DIM_OBJECT * MAX_DIM_OBJECT * sizeof(int));
	if(!Array1D)
	{
		printf("Failed to allocate memory for Array1D\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return;
	}
		
	for(int i = 0; i < numObjects; i++) // 8
	{

		convertTo1D_Object(&objects[i], Array1D);
		int dim = objects[i].objDim;
		MPI_Send(&dim, 1, MPI_INT, WORKER, 0, MPI_COMM_WORLD); // send the dim
		MPI_Send(Array1D, dim * dim, MPI_INT, WORKER, 0, MPI_COMM_WORLD); // send the 1D array of obj
		MPI_Send(&objects[i].objID, 1, MPI_INT, WORKER, 0, MPI_COMM_WORLD); // send ID of obj
		
	}
	free(Array1D);
}


void convertTo1D_Object(Object* obj, int* Array1D)
{
	for(int i = 0; i < obj->objDim; i++)
		for(int j = 0; j < obj->objDim; j++)
		{
			int index = i * obj->objDim + j;
			*Array1D = obj->objMatrix[index];
			Array1D++;
		}
}


void MPI_SendPicturesToWorker(Picture* picturesForWorker, int numPicturesForWorker)
{
	
	int* Array1D = (int*)malloc(MAX_DIM_PICTURE * MAX_DIM_PICTURE * sizeof(int));
	if(!Array1D)
	{
		printf("Failed to allocate memory for Array1D\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return;
	}
	
	for(int i = 0; i < numPicturesForWorker; i++)
	{
		
		convertTo1D_Picture(&picturesForWorker[i], Array1D);
		int dim = picturesForWorker[i].picDim * picturesForWorker[i].picDim;
		picturesForWorker[i].numObj = 0;
		MPI_Send(&picturesForWorker[i].picDim, 1, MPI_INT, WORKER, 0, MPI_COMM_WORLD);
		MPI_Send(Array1D, dim, MPI_INT, WORKER, 0, MPI_COMM_WORLD);
		MPI_Send(&picturesForWorker[i].picID, 1, MPI_INT, WORKER, 0, MPI_COMM_WORLD);	
		
	}
	free(Array1D);
}


void convertTo1D_Picture(Picture* pic, int* Array1D)
{
	for(int i = 0; i < pic->picDim; i++)
		for(int j = 0; j < pic->picDim; j++)
		{
			int index = i * pic->picDim + j;
			*Array1D = pic->picMatrix[index];
			Array1D++;
		}
}



Object* MPI_RecvObjectsFromMaster(int numObjects)
{
	Object* Obj = (Object*)malloc(numObjects * sizeof(Object));
	if(!Obj)
	{
		printf("Failed to allocate memory for Obj\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return NULL;
	}
	int* Array1D = (int*)malloc(MAX_DIM_OBJECT * MAX_DIM_OBJECT * sizeof(int));
	if(!Array1D)
	{
		printf("Failed to allocate memory for Array1D\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return NULL;
	}
	
	for(int i = 0; i < numObjects; i++)
	{
		int dim = 0;

		
		MPI_Recv(&dim, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		Obj[i].objDim = dim;
		int size = dim * dim;
		
		MPI_Recv(Array1D, size, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		MPI_Recv(&Obj[i].objID, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		Obj[i].objMatrix = (int*)malloc(size * sizeof(int));
		saveItAs2D_Object(Array1D, &Obj[i]);
		
		
	}
	free(Array1D);
	return Obj;
}


void saveItAs2D_Object(int* Array1D, Object* obj)
{
	for(int i = 0; i < obj->objDim; i++)
		for(int j = 0; j < obj->objDim; j++)
		{
			int index = i * obj->objDim + j; 
			obj->objMatrix[index] = Array1D[index];
		}	
}


Picture* MPI_RecvPicturesFromMaster(int numOfPic)
{
	Picture* Pic = (Picture*)malloc(numOfPic * sizeof(Picture));
	if(!Pic)
	{
		printf("Failed to allocate memory for Pic\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return NULL;
	}
	int* Array1D = (int*)malloc(MAX_DIM_PICTURE * MAX_DIM_PICTURE * sizeof(int));
	if(!Array1D)
	{
		printf("Failed to allocate memory for Array1D\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return NULL;
	}
	
	
	for(int i = 0; i < numOfPic; i++)
	{
		int dim = 0;

		
		MPI_Recv(&dim, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		Pic[i].picDim = dim;
		int size = dim * dim;
		
		MPI_Recv(Array1D, size, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		MPI_Recv(&Pic[i].picID, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		Pic[i].picMatrix = (int*)malloc(size * sizeof(int));
		saveItAs2D_Picture(Array1D, &Pic[i]);
	}
	free(Array1D);
	return Pic;
}



void saveItAs2D_Picture(int* Array1D, Picture* pic)
{
	for(int i = 0; i < pic->picDim; i++)
		for(int j = 0; j < pic->picDim; j++)
		{
			int index = i * pic->picDim + j;
			pic->picMatrix[index] = Array1D[index];
		}	
}



void MPI_SendResultsToMaster(Picture* Pic, int numPicturesForWorker)
{

	int* positions = (int*)malloc(DIFF_OBJ_REQ * 2 * sizeof(int));
	if(!positions)
	{
		printf("Failed to allocate memory for positions\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return;
	}
	
	
	for(int i = 0; i < numPicturesForWorker; i++)
	{
	
		int numObj = Pic[i].numObj;
		MPI_Send(&numObj, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
		int diffObj = DIFF_OBJ_REQ;
		
		MPI_Send(&Pic[i].foundObj, diffObj, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
		
		convertTo1D_positions(positions, &Pic[i]);
		MPI_Send(positions, diffObj * 2, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
		
	}
	int tag = 1;
	MPI_Send(&tag, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
	
	free(positions);

}



void convertTo1D_positions(int* positions, Picture* pic)
{
	for(int i = 0; i < DIFF_OBJ_REQ; i++)
		for(int j = 0; j < 2; j++)
		{
			*positions = pic->positions[i][j];
			positions++;
		}
}




int MPI_RecvResultsFromWorker(Picture* Pic, int numPicturesForWorker)
{
	int* positions = (int*)malloc(DIFF_OBJ_REQ * 2 * sizeof(int));
	if(!positions)
	{
		printf("Failed to allocate memory for Array1D\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return 0;
	}
	int* tag = (int*)malloc(sizeof(int));
	for(int i = 0; i < numPicturesForWorker; i++)
	{
		int* numObj = (int*)malloc(sizeof(int));
		MPI_Recv(numObj, 1, MPI_INT, WORKER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		Pic[i].numObj = *numObj;
		
		MPI_Recv(&Pic[i].foundObj, DIFF_OBJ_REQ, MPI_INT, WORKER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		MPI_Recv(positions, DIFF_OBJ_REQ * 2, MPI_INT, WORKER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		convertTo2D_positions(positions, &Pic[i]);
		free(numObj);
		
	}
	MPI_Recv(tag, 1, MPI_INT, WORKER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	int returnTag = *tag;
	free(positions);
	free(tag);
	return returnTag;
}


void convertTo2D_positions(int* positions, Picture* pic)
{
	for(int i = 0; i < DIFF_OBJ_REQ; i++)
		for(int j = 0; j < 2; j++)
		{
			int index = i * 2 + j;
			pic->positions[i][j] = positions[index];
		}
}

// void run_worker_rank() {
//     int numPicturesForWorker = 0, numObjects = 0;
//     Picture* picturesForWorker = NULL;
//     double start = 0.0, end = 0.0;
//     double execTime = 0.0;

//     MPI_Recv(&numPicturesForWorker, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//     MPI_Recv(&numObjects, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//     MPI_Recv(&calc_mathcing, 1, MPI_FLOAT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

//     printf("---------------------\n");
//     printf("Receiving %d pictures and %d objects in rank 1:\n", numPicturesForWorker, numObjects);

//     Picture* picturesForWorker = (Picture*)malloc(numPicturesForWorker * sizeof(Picture));

//     MPI_RecvObjectsFromMaster(objects, numObjects);
//     MPI_RecvPicturesFromMaster(picturesForWorker, numPicturesForWorker);

//     printf("---------------------\n");
//     printf("Starting to search pictures for objects in rank 1:\n");
//     start = MPI_Wtime();
//     #pragma omp parallel for num_threads(NUM_THREADS)
//     for(int i = 0; i < numPicturesForWorker; i++)
//     {            
//         findMatching(&picturesForWorker[i], objects, numObjects, calc_mathcing, WORKER);
//     }

//     end = MPI_Wtime();
//     printf("---------------------\n");
//     printf("Sending Results from rank 1 to rank 0:\n");

//     MPI_SendResultsToMaster(picturesForWorker, numPicturesForWorker);

//     execTime = end - start;
//     printf("---------------------\n");
//     printf("Exec time of worker program: %e or %f sec\n", execTime, execTime);

//     printf("Free allocation for rank 1\n");
//     free(picturesForWorker);
	
// }


