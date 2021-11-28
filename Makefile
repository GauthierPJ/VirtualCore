SRC  = $(wildcard *.c) #fichiers sources
OBJ  = $(SRC:.c=.o) #fichiers objets
CC   = gcc 
PROG = main

all : $(PROG)

$(PROG) : $(OBJ) #Compiliation des fichiers .c
	$(CC) $^ -o $@ -lm 

.c.o : #Compiliation des fichiers .o
	$(CC) -Wall -c $^ -o $@ -lm 

.PHONY : clean

clean   :
	rm -f $(OBJ)
