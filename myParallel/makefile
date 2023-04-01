# CC = mpicc
# CFLAGS = -Wall -Wextra -std=c99
# LIBS = -lm

# INCLUDES = func.h 
# OBJECTS  = func.o app.o

# .PHONY: all mpi omp clean run_mpi

# all: seq_mpi

# seq_mpi: $(OBJECTS)
# 	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o seq_mpi

# func_mpi.o: func.c $(INCLUDES)
# 	$(CC) $(CFLAGS) -DMPI -c func.c -o func_mpi.o

# app_mpi.o: app.c $(INCLUDES)
# 	$(CC) $(CFLAGS) -DMPI -c app.c -o app_mpi.o

# %.o: %.c $(INCLUDES)
# 	$(CC) $(CFLAGS) -c $< -o $@

# run_mpi:
# 	mpiexec -np 2 ./seq_mpi

# clean:
# 	$(RM) seq_mpi $(OBJECTS) func_mpi.o app_mpi.o

CC = mpicc
CFLAGS = -Wall -Wextra -std=c99 -fopenmp 
LIBS = -lm

INCLUDES = func.h parallel.h
OBJECTS  = func.o parallel.o app.o 
RM = rm -f

all: mpi

mpi: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o mpi

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@
	
run:
	mpirun -np 2 ./mpi
	
clean:
	$(RM) mpi $(OBJECTS)