#ifndef FUNC_H
#define FUNC_H
#include <stdio.h>
#define INPUT "input.txt"
#define OUTPUT "output.txt"
#define DIFF_OBJ_REQ 3
#define NUM_THREADS 4

typedef struct Picture
{
	int picID;
	int picDim;
	int* picMatrix;
	int foundObj[DIFF_OBJ_REQ];
	int numObj;
	int positions[DIFF_OBJ_REQ][2];
}Picture; //3 different position for (i,j)

typedef struct Object
{
	int objID;
	int objDim;
	int* objMatrix;
}Object;


/*searches through each Picture structure in pictures and for each one, searches for matches with each Object structure in objects.
It does this by iterating through all possible starting indices of the submatrix of picMatrix to compare with objMatrix.
 For each match, it stores the index of the matching Object in the Picture structure along with the starting indices of the submatrix where the match was found.
  If the number of matches found in a Picture structure reaches a predetermined threshold (DIFF_OBJ_REQ(3)),
   the function stops searching for matches in that Picture structure. The calc_matching parameter determines the similarity score threshold for considering a match.
    If a match is found with a similarity score less than or equal to calc_matching, it is considered a match.*/
void findMatching(Picture* pictures, Object* objects, int numOfObj, float calc_mathcing, int rank);
/*takes in two matrices (picMatrix and objMatrix) representing grayscale images, their respective dimensions (picDim and objDim), and two integers (p and o)
representing the starting indices for the submatrix of picMatrix to compare with objMatrix.
The function then calculates the similarity score between the two submatrices using the mean absolute percentage error (MAPE) formula.
The similarity score is the average MAPE between corresponding pixels in the submatrices.*/
float matching(int* picMatrix, int* objMatrix, int picDim, int objDim, int p, int o);

//read and scan the input.txt file then store it to use the paramters later 

int readInputFile(Picture** pictures, int* numOfPic, Object** objects, int* numOfObj, float* calc_mathcing);

/*reads the pictures' details from an input file.
 It takes an array of Picture objects, the number of pictures, and a file pointer as inputs.
It reads the ID, dimension, and matrix of each picture from the input file and stores them in the array of pictures.*/

int readPictures(Picture* pictures, int numOfPic, FILE* fp);

/* reads the objects' details from an input file.
 It takes an array of Object objects, the number of objects, and a file pointer as inputs.
It reads the ID, dimension, and matrix of each object from the input file and stores them in the array of objects.*/

int readObjects(Object* objects, int numOfObj, FILE* fp);
/*writes the output to a file.
It takes an array of Picture objects and the number of pictures as inputs. 
It opens an output file and writes the objects' IDs and their corresponding positions in the picture if three different objects are found.
 If not, it writes "No three different Objects were found" for that picture.*/

int writeToOutputFile(const Picture* pictures, int numOfPictures);
//free structs and pointers 

void freeAllocations(Picture* pictures, int numOfPic, Object* objects, int numOfObj);
/*combines information from two arrays of Picture structures into a single array
The function loops over each Picture structure in pictures and checks if it matches a Picture structure in either picturesForMaster or picturesForWorker based on their unique picID.
If a match is found, the function updates the relevant fields in the pictures structure with the information from the matching structure in picturesForMaster or picturesForWorker.*/

void mergePic(Picture* pictures, Picture* picturesForMaster, Picture* picturesForWorker, int numOfPic, int numPicturesForWorker, int numPicturesForMaster);

#endif
