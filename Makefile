include ./Makefile.global

DIRS = ts
OBJ = main.o tsutils.o bitutils.o
LINKOBJ = ts/adaption_field.o ts/MPEG_transport_stream.o ts/program_association_section.o ts/transport_packet.o $(OBJ)

all: $(OBJ) compile
	$(CC) $(CFLAGS) -o tsanalyzer $(LINKOBJ) $(LDFLAGS)
	
compile:
	for i in $(DIRS); do make -C $$i; done

clean: clean-all
	rm *.o tsanalyzer & true
	
clean-all:
	for i in $(DIRS); do make -C $$i clean; done
