include ../../Makefile.global

OBJ = main.o

all: clean $(OBJ)
	$(CC) $(CFLAGS) -o a $(OBJ) -lm

clean:
	rm *.o a || true
