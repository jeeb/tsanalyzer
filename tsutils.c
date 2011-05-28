#include "common.h"

void sync_to_syncbyte(FILE *f){
	int oldsize = ftell(f);

	int chk = 0;
	while (!feof(f) && chk != SYNC_BYTE){
		chk = fgetc(f);
	}

	int newsize = ftell(f);
	
	if (oldsize != newsize) { fseek(f,-1,SEEK_CUR); oldsize--; }
	
	#ifdef DEBUG
		printf("synced to 0x47. skipped %d byte(s).\n",newsize-oldsize);
	#endif
}
