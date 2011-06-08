#include "../common.h"

void MPEG_transport_stream(char* filename, struct transport_stream *ts){
	// global declarations
	FILE *f;
	char* data = (char*)malloc(TS_MAXDATA);
	
	// trying to open the file
	if (!(f = fopen(filename,"r"))){
		printf("(CRITICAL) error while opening file '%s'. Does the file really exist?\n",filename);
		return;
	}
	
	// synchronizing to the first SYNC_BYTE.
	sync_to_syncbyte(f);
	
	// checking if the file closed after attempting to sync 
	/*if (feof(f)){
		fclose(f);
		printf("(CRITICAL) EOF before mainloop.\n");
		return;
	}*/
	
	// loop for all packets
	while (!feof(f)){
		fread(data,TS_MAXDATA,1,f);
		transport_packet(data,ts);
		//sync_to_syncbyte(f);
	}
	
	// close file.
	fclose(f);
}