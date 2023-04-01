#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "func.h"


int main()
{
	float calc_mathcing = 0.0;
	int numOfPic = 0, numOfObj = 0;
	Picture* pictures = NULL;
	Object* objects = NULL;
	clock_t start, end;
	double execTime = 0.0;
	printf("--------sequential program--------\n");
	if (!readInputFile(&pictures, &numOfPic, &objects, &numOfObj, &calc_mathcing))
	{
		printf("Failed to read from Input file\n");
		return 0;
	}

	printf("Starting to search pictures for objects:\n");
	// find the matching for each object in the picture
	start = clock();
	findMatching(pictures, numOfPic, objects, numOfObj, calc_mathcing);
	end = clock();
	if (!writeToOutputFile(pictures, numOfPic))
	{
		printf("Failed to write to output file\n");
		return 0;
	}

	freeAllocations(pictures, numOfPic, objects, numOfObj);

	execTime = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("---------------------\n");
	printf("Exec time of sequential program: %f sec\n", execTime);
	return 0;
}

