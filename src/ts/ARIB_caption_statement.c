#include "../common.h"

uint8_t ARIB_caption_statement(char* data, uint8_t pos){
	uint8_t TMD = (data[pos] & 0xC0) >> 6;
	uint8_t reserved1 = data[pos] & 0x3F;
	pos++;
	
	printf("(ARIB-CS): TMD: %X | reserved1: %X | ",TMD,reserved1);
	if (TMD == 0x1 || TMD == 0x02){
		//uint64_t STM = (((data[pos] << 32) & 0xFF00000000) | ((data[pos+1] << 24) & 0xFF000000) | ((data[pos+2]<<16) & 0xFF0000) | ((data[pos+3]<<8) & 0xFF00) | (data[pos+4] & 0xF0)) >> 4;
		uint64_t STM = (((data[pos] & 0xFF00000000) << 32) | ((data[pos+1]& 0xFF000000 )<< 24 ) | ((data[pos+2]& 0xFF0000) <<16) | ((data[pos+3] & 0xFF00) <<8) | (data[pos+4] & 0xF0)) >> 4;
		uint8_t reserved2 = data[pos+4] & 0x0F;
		printf(" STM: %X | reserved2: %X",STM,reserved2);
		pos=pos+5;
	}
	
	//uint32_t dull = ((data[pos] << 16) & 0xFF0000) | ((data[pos+1] << 8) & 0xFF00) | (data[pos+2] & 0xFF);
	uint32_t dull = ((data[pos] << 16) & 0xFF0000) | ((data[pos+1] << 8) & 0xFF00) | (data[pos+2] & 0xFF);
	uint32_t dullcounter = dull;
	
	pos = pos + 3;
	
	printf(" DULL: %X\n",dull);	
	
	while (dullcounter != 0){
		uint8_t oldpos = pos;
		pos = ARIB_data_unit(data,pos,dull);
		dullcounter = dullcounter - (pos - oldpos);
		//printf("dullcounter %d\n",dullcounter);
	}
	return pos;
}