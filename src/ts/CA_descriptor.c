#include "../common.h"

uint8_t CA_descriptor(char* data, uint8_t pos){
	// AAAAAAAA
	// BBBBBBBB
	// CCCCCCCC
	// CCCCCCCC
	// DDDEEEEE
	// EEEEEEEE
	uint8_t DL = data[pos+1];
	uint16_t CASID = (data[pos+2]<<8)|(data[pos+3]&&0xff);
	uint8_t reserved1 = (data[pos+4]&0xE0)>>5;
	uint16_t CAPID = (data[pos+4]&0x1f)<<8|(data[pos+5]&0xff);
	uint8_t i;
	printf("(CA_descriptor) CASID: %d/0x%X | (reserved1: 0x%X) | CAPID: %d/0x%X\n",CASID,CASID,reserved1,CAPID,CAPID);
	
	pos = pos + 6;
	i=DL-4;
	while (i>0){
		bitout_ui8(data[pos]);
		printf("\n");
		pos++;
		i = i - 1;
	}
	
	return pos;
}