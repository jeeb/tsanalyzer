#include "../common.h"
uint8_t descriptor(char* data, uint8_t datapos){
	uint8_t DT = data[datapos];
	uint8_t DL = data[datapos+1];
	datapos = datapos + 2;
	printf("(DESCRIPTOR) descriptor type: 0x%X (%d) | length: %d/0x%X\n",DT,DT,DL,DL);
	if (DT == 9){
		return CA_descriptor(data,datapos-2);
	}
	
	return datapos + DL;
}