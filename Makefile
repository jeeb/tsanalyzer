include ./Makefile.global

DIRS = ts
OBJ = main.o
LINKOBJ = main.o ts/adaption_field.o ts/MPEG_transport_stream.o ts/program_association_section.o ts/transport_packet.o

all: $(OBJ) compile
	$(CC) $(CFLAGS) -o tsanalyzer $(LINKOBJ) $(LDFLAGS)

compile:
	for i in $(DIRS); do make -c $$i; done

