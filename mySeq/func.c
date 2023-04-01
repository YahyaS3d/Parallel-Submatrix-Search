#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "func.h"

int writeToOutputFile(const Picture* pictures, int numOfPic)
{//write the result in output.txt 
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
		if (pictures[i].numObj >= DIFF_OBJ_REQ )
		{
			if (pic.foundObj[0] != pic.foundObj[1] && pic.foundObj[1] != pic.foundObj[2])
			{
				fprintf(fp, "Picture %d: found Objects: %d Position(%d,%d); %d Position(%d,%d); %d Position(%d,%d)\n",
					picID, pic.foundObj[0], pic.positions[0][0], pic.positions[0][1],
					pic.foundObj[1], pic.positions[1][0], pic.positions[1][1],
					pic.foundObj[2], pic.positions[2][0], pic.positions[2][1]);
				printf("Picture %d: found Objects: %d Position(%d,%d); %d Position(%d,%d); %d Position(%d,%d)\n",
					picID, pictures[i].foundObj[0], pic.positions[0][0], pic.positions[0][1],
					pic.foundObj[1], pic.positions[1][0], pic.positions[1][1],
					pic.foundObj[2], pic.positions[2][0], pic.positions[2][1]);
			}
		}
		if (pic.numObj < DIFF_OBJ_REQ  || (pic.foundObj[1] == pic.foundObj[0] || pic.foundObj[1] == pic.foundObj[2] || pic.foundObj[2] == pic.foundObj[0]))
		{
			fprintf(fp, "Picture %d: No three different Objects were found\n", picID);
			printf("Picture %d: No three different Objects were found\n", picID);
		}

	}
	fclose(fp);
	return 1;
}





int readObjects(Object* objects, int numOfObj, FILE* fp)
{
	printf("Reading Objects ==> Number of objects in the input file is %d:\n", numOfObj);
	printf("----------Scanning Objects!!----------\n");
	for (int i = 0; i < numOfObj; i++)
	{
		// get object i Id
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

		int dim = objects[i].objDim * objects[i].objDim; //N x N 

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
		pictures[i].picMatrix  = (int*)malloc(dim * sizeof(int));
		if (!pictures[i].picMatrix )
		{
			printf("Failed to allocate memory for picture\n");
			return 0;
		}
		for (int j = 0; j < dim; j++)
		{
			if (fscanf(fp, "%d", &pictures[i].picMatrix [j]) != 1)
			{
				printf("Failed to read Picture item at index %d\n", j);
				return 0;
			}
		}
		printf("Success in reading Picture %d with dimention %d\n", pictures[i].picID, pictures[i].picDim);
	}
	return 1;
}
void freeAllocations(Picture* pictures, int numOfPic, Object* objects, int numOfObj)
{
	for (int i = 0; i < numOfPic; i++)
	{
		// free picMatrix  for picture i
		free(pictures[i].picMatrix );
	}
	// free allocated memory for pictures
	free(pictures);
	for (int i = 0; i < numOfObj; i++)
	{
		// free objMatrix for object i
		free(objects[i].objMatrix);
	}
	// free allocated memory for objects
	free(objects);
}

int readInputFile(Picture** pictures, int* numOfPic, Object** objects, int* numOfObj, float* calc_mathcing)
{
	FILE* fp = fopen(INPUT, "r");
	if (!fp)
	{
		printf("Could not open the file\n");
		return 0;
	}
	printf("Start Reading Input File\n");

	//read the matching value
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
	if (fscanf(fp, "%d", numOfObj) != 1)
	{
		printf("Failed to read number of objects\n");
		return 0;
	}
	// allocate memory for objects
	*objects = (Object*)malloc(*numOfObj * sizeof(Object));
	if (!*objects)
	{
		printf("Failed to allocate memory for objects\n");
		return 0;
	}
	//reading from the file the objects: we read the ID and the dim of each object
	if (!readObjects(*objects, *numOfObj, fp))
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
float matching(int* picMatrix , int* objMatrix, int picDim, int objDim, int p, int o)
{
	//Initialize matching value 
	float matching_val = 0.0;
	//// Calculate the total number of pixels in the object matrix
	int objSize = objDim * objDim;
	//Initialize  difference to calculate the matching percent 
	float diff = 0.0;
	for (int i = 0; i < objDim; i++)
	{
		for (int j = 0; j < objDim; j++)
		{
			// Calculate the index of the corresponding pixel in the picture matrix
			int picIndex = (p + i) * picDim + (o + j);
			int objIndex = i * objDim + j;
			// Retrieve the pixel values from the picture and object matrices
			int picPix = picMatrix [picIndex];
			int objPix = objMatrix[objIndex];
			diff = (float)abs((picPix - objPix) / picPix);
			matching_val += diff;
		}
	}
	//Calculate the average difference and return it as the matching value
	matching_val /= objSize;
	return matching_val;
}



void findMatching(Picture* pictures, int numOfPic, Object* objects, int numOfObj, float calc_mathcing)
{
	for (int i = 0; i < numOfPic; i++)
	{
		Picture* pic = &pictures[i];
		int picDim = pictures[i].picDim;
		int* picMatrix  = pictures[i].picMatrix ;
		// number of objects found in the current picture
		pic->numObj = 0;
			// iterate through all the objects
		for (int j = 0; j < numOfObj; j++)
		{
		// dimensions of the current object
		int objDim = objects[j].objDim;
		// matrix of the current object
		int* objMatrix = objects[j].objMatrix;
		// ID of the current object
		int objID = objects[j].objID;
		// flag for indicating if the current object is found in the current picture
		int found = 0;
		// maximum values for p and o
		int maxI, maxJ;
		maxI = maxJ = picDim - objDim;

		// iterate through all the possible positions of the object in the picture
		for (int p = 0; p <= maxI && pic->numObj < DIFF_OBJ_REQ ; p++)
		{
			for (int o = 0; o <= maxJ && pic->numObj < DIFF_OBJ_REQ ; o++)
			{
				// calculate the matching value between the current object and the current position in the picture
				float tempMatching = matching(picMatrix , objMatrix, picDim, objDim, p, o);

				// if the matching value is less than or equal to the given threshold, the object is found in the picture
				if (tempMatching <= calc_mathcing) //found
				{
					// if the object is found for the first time or it was found at a different location, add it to the list of found objects in the picture
					if (pic->numObj == 0 || pic->foundObj[pic->numObj - 1] != objID)
					{
						pic->foundObj[pic->numObj] = objID;
						pic->positions[pic->numObj][0] = p;
						pic->positions[pic->numObj][1] = o;
						pic->numObj++;
					}
					// if the maximum number of objects is already found, set the flag to indicate that the object is found and break out of the loops
					if (pic->numObj == DIFF_OBJ_REQ )
					{
						found = 1;
						break;
					}
				}
			}
			// if the object is found, break out of the loop
			if (found)
				break;
		}
		// if the number of remaining objects is less than the number of objects required to find, break out of the loop
		if (numOfObj - j - 1 < DIFF_OBJ_REQ  - pic->numObj)
			break;
		// if the object is found, break out of the loop
		if (found)
			break;
	}

}
}

