include ../../Makefile.global

APPNAME = captionparser
OBJ = main.o

all: clean $(OBJ)
	$(CC) $(CFLAGS) -o $(APPNAME) $(OBJ) -lm

clean:
	rm *.o $(APPNAME) || true
