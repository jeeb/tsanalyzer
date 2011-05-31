#include "../common.h"
uint8_t CA_section(char* data, uint8_t pos){
	// AAAAAAAA
	// B0CCDDDD
	// DDDDDDDD
	uint8_t TID = data[pos];
	uint8_t SSI = data[pos+1] & 0x80 >> 7;
	uint8_t reserved1 = data[pos+1] & 0x30 >> 4;
	uint16_t SL = (data[pos+1] & 0x0F) << 8 | (data[pos+2] & 0xFF);
	uint32_t reserved2 = (data[pos+3]) << 16 | (data[pos+4] & 0xFF00) << 8 | (data[pos+5] & 0xC0);
	uint8_t VN = data[pos+5] & 0x3E >> 1;
	uint8_t CNI = data[pos+5] & 0x01;
	uint8_t SN = data[pos+6];
	uint8_t LSN = data[pos+7];
	
	pos = pos + 8;
	
	printf("(CAT) TID: 0x%X | SSI: 0x%X | (reserved1: 0x%X) | SL: 0x%X | (reserved2: 0x%X) | VN: 0x%X | CNI: 0x%X | SN: 0x%X | LSN: 0x%X\n",TID,SSI,reserved1,SL,reserved2,VN,CNI,SN,LSN);
	
	uint16_t i;
	if ((SL & 0xC00) >> 10 != 0){
		printf("(CAT) conflict with specification!! [first 2 bits in SL must be '0']");
		#ifdef DEBUG
			printf(" (");
			bitout_ui16(SL);
			printf(")");
		#endif
		printf("\n");
		return 184;	
	
	}
	if (SL >= 0x3fd){
		printf("(CAT) conflict with specification!! [SL must be < 0x3fd]\n");
		return 184;
	}
	
	uint16_t SLcounter = SL;
	SLcounter = SLcounter - 5;
	
	printf("SLcounter: %u",SLcounter);
	while (SLcounter > 4){
		uint8_t oldpos = pos;
		pos = descriptor(data, pos);
		SLcounter = SLcounter - (pos - oldpos);
		printf("SLcounter: %u",SLcounter);
	}
	printf("SLcounter: %u",SLcounter);
	uint32_t crc32 = ((data[pos+0] & 0xFF000000) << 24) | ((data[pos+1] & 0xFF0000) << 16) | ((data[pos+2] & 0xFF00) << 8) | (data[pos+3] & 0xFF) ;
	printf("(CAT) CRC32: %u/%X %X %X %X\n",crc32,(data[pos+0]) , (data[pos+1]) , (data[pos+2]) , (data[pos+3]));
	pos = pos+4; 
	return pos;
}