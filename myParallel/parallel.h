#ifndef PARALLEL_H
#define PARALLEL_H

#include "func.h"

#define MASTER 0 
#define WORKER 1
#define MAX_DIM_FOR_WORKER 300
#define MAX_DIM_OBJECT 300
#define MAX_DIM_PICTURE 500


void MPI_SendObjectsToWorker(Object* object, int numObjects);
void MPI_SendPicturesToWorker(Picture* picturesForWorker, int numPicturesForWorker);


void convertTo1D_Object(Object* obj, int* Array1D);
void convertTo1D_Picture(Picture* pic, int* Array1D);


Object* MPI_RecvObjectsFromMaster(int numObjects);
Picture* MPI_RecvPicturesFromMaster(int numOfPic);


void saveItAs2D_Object(int* Array1D, Object* obj);
void saveItAs2D_Picture(int* Array1D, Picture* pic);


void MPI_SendResultsToMaster(Picture* Pic, int numPicturesForWorker);
int MPI_RecvResultsFromWorker(Picture* Pic, int numPicturesForWorker);

void convertTo1D_positions(int* positions, Picture* pic);
void convertTo2D_positions(int* positions, Picture* pic);

#endif
