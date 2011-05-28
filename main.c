#include "common.h"

#ifdef DEBUG
	#define VERBOSE 1
#endif

int main(int argc, char** argv){
	if (argc < 2){
		printf("usage: %s filename\n",argv[0]);
		return 1;
	}
	MPEG_transport_stream(argv[1]);

	return 0;
}
