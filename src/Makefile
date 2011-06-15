include ../Makefile.global

DIRS = ts captions/ARIB
OBJ = main.o tsutils.o bitutils.o
TSOBJ = ts/adaption_field.o ts/MPEG_transport_stream.o ts/program_association_section.o ts/transport_packet.o ts/PES_packet.o ts/CA_section.o ts/CA_descriptor.o ts/TS_program_map_section.o ts/descriptor.o ts/ARIB_caption_statement.o ts/ARIB_caption_management.o ts/ARIB_data_group.o ts/ARIB_data_unit.o
CAPOBJ = captions/ARIB/parse_caption_payload.o
LINKOBJ =  $(TSOBJ) $(CAPOBJ) $(OBJ)

all: $(OBJ) compile
	$(CC) $(CFLAGS) -o tsanalyzer $(LINKOBJ) $(LDFLAGS)
	
compile:
	for i in $(DIRS); do make -C $$i; done

clean: clean-all
	rm *.o tsanalyzer || true
	
clean-all:
	for i in $(DIRS); do make -C $$i clean; done
