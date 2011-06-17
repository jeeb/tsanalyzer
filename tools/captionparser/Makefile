include ../../Makefile.global

APPNAME = captionparser
OBJ = main.o
CFLAGS = -I./libaribb24captions/ 
LIBS = -lm -L./libaribb24captions/ -laribb24captions

all: clean $(OBJ)
	$(CC) $(CFLAGS) -o $(APPNAME) $(OBJ) $(LIBS)

clean:
	rm *.o $(APPNAME) || true
