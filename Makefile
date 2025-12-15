CC = gcc
CFLAGS = -Wall -g

SRC = dict2.c tree.c record.c csv.c result.c bit.c edit_dist.c
OBJ = $(SRC:.c=.o)
EXE = dict2

# The first target:
$(EXE): $(OBJ) 
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LDLIBS)

clean:
	rm -f $(OBJ) $(EXE)
