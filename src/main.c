#include "common.h"

#ifdef DEBUG
	#define VERBOSE 1
#endif

int main(int argc, char** argv){
	if (argc < 2){
		printf("usage: %s filename\n",argv[0]);
		return 1;
	}
	struct transport_stream *ts = malloc(sizeof(struct transport_stream));
	ts->network_pid_set = 0;
	ts->serial_number_set = 0;
	ts->programs = NULL;
	MPEG_transport_stream(argv[1],ts);

	return 0;
}
