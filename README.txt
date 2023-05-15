Authored by Yahya Saad
==Submatrix Search For Pircure Object==
This project deals with a set of Pictures and Objects of different sizes. Each member of the matrix represents a "color". The range of possible colors is [1, 100]. Position(I, J) defines a coordinate of the upper left corner of the Object into Picture.

For each pair of overlapping members p and o of the Picture and Object, the program calculates a relative difference. The total difference is defined as an average of all relative differences for all overlapping members for given Position(I, J) of the Object into Picture. We call it Matching(I, J).

The goal of the project is to find if the given picture contains at least three objects from the given object set with a Matching value less than a given one. 

==Configuration==
in this program i used MPI and openMP to parallel the solution


==Files and Folders== 
this project has 2 starter points: 
1- MySeq: contains the files to start the sequentail solution 
2- MyParallel: contains the files to start the parallel solution, contains full explaination how the solution got parallel 

==How to run?== 
in termenal 
-cd into the folder to make a run 
-compile: make all 
-run: make run 
-clean and reset point: make clean 

==Result and show proccess== 


| Code Type     | Execution Time|
| ------------- | ------------- |
| Sequential    | 19.882 seconds|
| Parallel      | 9.822 seconds |
  
==References and Links==
* Introduction to Parallel
Programming with MPI and
OpenMP : 
https://princetonuniversity.github.io/PUbootcamp/sessions/parallel-programming/Intro_PP_bootcamp_2018.pdf 

*Hybrid MPI and OpenMP
Parallel Programming : 
https://www.openmp.org/wp-content/uploads/HybridPP_Slides.pdf 

*Parallel Programming 2020: Lecture 13 - Hybrid Programming with MPI and OpenMP: 
https://youtu.be/OHijtcMGm9I 


