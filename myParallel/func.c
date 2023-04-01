#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "func.h"
#include <omp.h>

void mergePic(Picture* pictures, Picture* picturesForMaster, Picture* picturesForWorker, int numOfPic, int numPicturesForWorker, int numPicturesForMaster)
{
    int j = 0, k = 0;

    #pragma omp parallel for
    for (int i = 0; i < numOfPic; i++)
    {
		 //private copies of the j and k variables for each thread
        int thread_j = j, thread_k = k;

        if(thread_j != numPicturesForMaster)
        {
            if(pictures[i].picID == picturesForMaster[thread_j].picID)
            {
                pictures[i].numObj = picturesForMaster[thread_j].numObj;
                for (int l = 0; l < DIFF_OBJ_REQ; l++)
                    pictures[i].foundObj[l] = picturesForMaster[thread_j].foundObj[l];

                for (int o = 0; o < DIFF_OBJ_REQ; o++)
                    for (int p = 0; p < 2; p++)
                        pictures[i].positions[o][p] = picturesForMaster[thread_j].positions[o][p];
                thread_j++;
            }
        }

        if(thread_k != numPicturesForWorker)
        {
            if(pictures[i].picID == picturesForWorker[thread_k].picID)
            {
                pictures[i].numObj = picturesForWorker[thread_k].numObj;
                for (int l = 0; l < DIFF_OBJ_REQ; l++)
                    pictures[i].foundObj[l] = picturesForWorker[thread_k].foundObj[l];

                for (int o = 0; o < DIFF_OBJ_REQ; o++)
                    for (int p = 0; p < 2; p++)
                        pictures[i].positions[o][p] = picturesForWorker[thread_k].positions[o][p];
                thread_k++;
            }
        }

        // Update the shared variables after the loop iteration
		//avoid using a critical section to update the shared variables, which can improve performance.
        #pragma omp atomic write
        j = thread_j;
        #pragma omp atomic write
        k = thread_k;
    }
}




void freeAllocations(Picture* pictures, int numOfPic, Object* objects, int numObjects)
{
	for (int i = 0; i < numOfPic; i++)
	{
		// free picMatrix for picture i
		free(pictures[i].picMatrix);
	}
	// free allocated memory for pictures
	free(pictures);
	for (int i = 0; i < numObjects; i++)
	{
		// free objMatrix for object i
		free(objects[i].objMatrix);
	}
	// free allocated memory for objects
	free(objects);
}

int writeToOutputFile(const Picture* pictures, int numOfPic)
{
	FILE* fp = fopen(OUTPUT, "w");
	if (!fp)
	{
		printf("Could not open file\n");
		return 0;
	}
	for (int i = 0; i < numOfPic; i++)
	{
		Picture pic = pictures[i];
		int picID = pictures[i].picID;
		if (pictures[i].numObj >= DIFF_OBJ_REQ)
		{
			if (pic.foundObj[0] != pic.foundObj[1] && pic.foundObj[1] != pic.foundObj[2])
			{
				fprintf(fp, "Picture %d: found Objects: %d Position(%d,%d); %d Position(%d,%d); %d Position(%d,%d)\n",
					picID, pic.foundObj[0], pic.positions[0][0], pic.positions[0][1],
					pic.foundObj[1], pic.positions[1][0], pic.positions[1][1],
					pic.foundObj[2], pic.positions[2][0], pic.positions[2][1]);
				printf("Saving info for Picture %d to file\n", picID);
				printf("Picture %d: found Objects: %d Position(%d,%d); %d Position(%d,%d); %d Position(%d,%d)\n",
					picID, pictures[i].foundObj[0], pic.positions[0][0], pic.positions[0][1],
					pic.foundObj[1], pic.positions[1][0], pic.positions[1][1],
					pic.foundObj[2], pic.positions[2][0], pic.positions[2][1]);
			}
		}
		if (pic.numObj < DIFF_OBJ_REQ || (pic.foundObj[1] == pic.foundObj[0] || pic.foundObj[1] == pic.foundObj[2] || pic.foundObj[2] == pic.foundObj[0]))
		{
			fprintf(fp, "Picture %d: No three different Objects were found\n", picID);
			printf("Saving info for Picture %d to file\n", picID);
			printf("Picture %d: No three different Objects were found\n", picID);
		}

	}
	fclose(fp);
	return 1;
}





int readObjects(Object* objects, int numObjects, FILE* fp)
{
	printf("Reading Objects ==> Number of objects in the input file is %d:\n", numObjects);
	printf("----------Scanning Objects!!----------\n");
	for (int i = 0; i < numObjects; i++)
	{
		// get object i ID
		if (fscanf(fp, "%d", &objects[i].objID) != 1)
		{
			printf("Failed to read Object ID\n");
			return 0;
		}
		// get object i dimension
		if (fscanf(fp, "%d", &objects[i].objDim) != 1)
		{
			printf("Failed to read Object dimension\n");
			return 0;
		}

		int dim = objects[i].objDim * objects[i].objDim;

		// allocate memory for the mat of object i
		objects[i].objMatrix = (int*)malloc(dim * sizeof(int));
		if (!objects[i].objMatrix)
		{
			printf("Failed to allocate memory for object\n");
			return 0;
		}

		//object items
		for (int j = 0; j < dim; j++)
		{
			if (fscanf(fp, "%d", &objects[i].objMatrix[j]) != 1)
			{
				printf("Failed to read Object item at index %d\n", j);
				return 0;
			}
		}
		printf("Success in reading Object %d with dimention %d\n", objects[i].objID, objects[i].objDim);
	}
	return 1;
}

int readPictures(Picture* pictures, int numOfPic, FILE* fp)
{
	printf("Reading Pictures ==> Number of pictures in the input file is %d:\n", numOfPic);
	printf("----------Scanning Pictures!!----------\n");
	for (int i = 0; i < numOfPic; i++)
	{
		// get picture i ID
		if (fscanf(fp, "%d", &pictures[i].picID) != 1)
		{
			printf("Failed to read Picture ID\n");
			return 0;
		}
		// get picture i Dim
		if (fscanf(fp, "%d", &pictures[i].picDim) != 1)
		{
			printf("Failed to read Picture Dim\n");
			return 0;
		}

		int dim = pictures[i].picDim * pictures[i].picDim;

		// allocate memory for the mat of picture i
		pictures[i].picMatrix = (int*)malloc(dim * sizeof(int));
		if (!pictures[i].picMatrix)
		{
			printf("Failed to allocate memory for picture\n");
			return 0;
		}

		//picture items
		for (int j = 0; j < dim; j++)
		{
			if (fscanf(fp, "%d", &pictures[i].picMatrix[j]) != 1)
			{
				printf("Failed to read Picture item at index %d\n", j);
				return 0;
			}
		}
		printf("Success in reading Picture %d with dimention %d\n", pictures[i].picID, pictures[i].picDim);
	}
	return 1;
}


int readInputFile(Picture** pictures, int* numOfPic, Object** objects, int* numObjects, float* calc_mathcing)
{
	FILE* fp = fopen(INPUT, "r");
	if (!fp)
	{
		printf("Could not open the file\n");
		return 0;
	}
	printf("Starting to Read Input File\n");
	if (fscanf(fp, "%f", calc_mathcing) != 1)
	{
		printf("Failed to read matching value\n");
		return 0;
	}
	printf("Matching value: %f\n", *calc_mathcing);

	// read number of pictures should be 5
	if (fscanf(fp, "%d", numOfPic) != 1)
	{
		printf("Failed to read number of pictures\n");
		return 0;
	}
	// allocate memory for pictures
	*pictures = (Picture*)malloc(*numOfPic * sizeof(Picture));
	if (!*pictures)
	{
		printf("Failed to allocate memory for pictures\n");
		return 0;
	}
	// reading from the file the pictures: we read the ID and the dim of each picture
	if (!readPictures(*pictures, *numOfPic, fp))
	{
		printf("Failed reading pictures from file\n");
		free(*pictures);
		fclose(fp);
		return 0;
	}

	//read number of objects should be 8 objects 
	if (fscanf(fp, "%d", numObjects) != 1)
	{
		printf("Failed to read number of objects\n");
		return 0;
	}
	// allocate memory for objects
	*objects = (Object*)malloc(*numObjects * sizeof(Object));
	if (!*objects)
	{
		printf("Failed to allocate memory for objects\n");
		return 0;
	}
	//reading from the file the objects: we read the ID and the dim of each object
	if (!readObjects(*objects, *numObjects, fp))
	{
		printf("Failed to read objects from file\n");
		free(*objects);
		fclose(fp);
		return 0;
	}
	printf("---------------------\n");
	fclose(fp);
	return 1;
}

float matching(int* picMatrix, int* objMatrix, int picDim, int objDim, int p, int o) // running on all the obj to find the avg matching value
{
	float matching_val = 0.0;
	int objSize = objDim * objDim; // the hole size of the obj
	float diff = 0.0;
	
	#pragma omp parallel for reduction(+:matching_val)
	for (int i = 0; i < objDim; i++)
	{
		for (int j = 0; j < objDim; j++)
		{
			int picIndex = (p + i) * picDim + (o + j); // getting the index of the picture
			int objIndex = i * objDim + j; //geting the index of the object
			int picPix = picMatrix[picIndex]; // the pixel of the picture
			int objPix = objMatrix[objIndex]; // the pixel of the object
			diff = (float)abs((picPix - objPix) / picPix); // diff = abs((p - o) / p)
			matching_val += diff; // diff for each pixel += the matching value for the object
		}
	}
	
	matching_val /= objSize; // matching value being div by the obj size as of the avg matching val per one pixel
	return matching_val;
}


void findMatching(Picture* pictures, Object* objects, int numObjects, float calc_mathcing, int rank)
{
		Picture* pic = pictures;
		int picDim = pictures->picDim;
		int* picMatrix = pictures->picMatrix;
		int picID = pictures->picID;
		pic->numObj = 0;
		printf("Rank %d searching picture %d\n", rank, picID);	
		for (int j = 0; j < numObjects; j++)
		{
			int objDim = objects[j].objDim;
			int* objMatrix = objects[j].objMatrix;
			int objID = objects[j].objID;
			int found = 0; // found is playing a boolean role in this function
			int maxI, maxJ;
			maxI = maxJ = picDim - objDim; // the max pixel we can run for the picture
		


			for (int p = 0; p <= maxI && pic->numObj < DIFF_OBJ_REQ; p++) // running up to maxI that there can be and if number of unique obj is 3 finish
			{
				for (int o = 0; o <= maxJ && pic->numObj < DIFF_OBJ_REQ; o++) // same us above just for maxJ
				{
					float tempMatching = matching(picMatrix, objMatrix, picDim, objDim, p, o);

					if (tempMatching <= calc_mathcing) // found obj
					{

						printf("\tObject %d: found in picture %d at position (%d,%d)\n",objID, picID, p, o);
						if (pic->numObj == 0 || pic->foundObj[pic->numObj - 1] != objID)
						{
							pic->foundObj[pic->numObj] = objID; // saving the obj id in the pic struct in the array of foundObj 
							pic->positions[pic->numObj][0] = p; // saving cord x
							pic->positions[pic->numObj][1] = o; // saving cord y
							pic->numObj++;
						}
						if (pic->numObj == DIFF_OBJ_REQ) // found 3 unique obj in pic
						{
							found = 1;
							break; // breaking the third for loop
						}
						
					}
				}
				if (found) // breaking the seconf for loop
					break;
			}
			if (numObjects - j - 1 < DIFF_OBJ_REQ - pic->numObj) // if there are not left too many obj to search break e.g if we found 1 obj and only one left to find just break
				break;
			if (found) // breaking the first for loop no need to search for more obj
				break;
		}
}

