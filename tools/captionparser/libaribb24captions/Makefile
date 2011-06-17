include ../../../Makefile.global

LIBNAME = libaribb24captions
OBJ = string-utils.o libaribb24captions.o pts.o

all: clean $(OBJ)
	ar crs $(LIBNAME).a $(OBJ)

clean:
	rm *.o $(LIBNAME).a || true
