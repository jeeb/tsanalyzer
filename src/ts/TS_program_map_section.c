#include "../common.h"
uint8_t TS_program_map_section(char* data, uint8_t pos){
	// 0: AAAAAAAA
	// 1: B0CCDDDD
	// 2: DDDDDDDD
	// 3: EEEEEEEE
	// 4: EEEEEEEE
	// 5: FFGGGGGH
	// 6: IIIIIIII
	// 7: JJJJJJJJ
	// 8: KKKLLLLL
	// 9: LLLLLLLL
	//10: MMMMNNNN
	//11: NNNNNNNN
	
	uint16_t i,j;
	
	uint8_t TID = data[pos];
	uint8_t SSI = data[pos+1]&0x80 >> 7;
	uint8_t reserved1 = data[pos+1] & 0x30 >> 4;
	uint16_t SL = (data[pos+1] & 0x0F) << 8 | (data[pos+2] & 0xFF);
	uint16_t PN = (data[pos+3]) << 8 | (data[pos+4] & 0xFF);
	uint8_t reserved2 = data[pos+5] & 0xC0 >> 6;
	uint8_t VN = data[pos+5] & 0x3E >> 1;
	uint8_t CNI = data[pos+5] & 0x01;
	uint8_t SN = data[pos+6];
	uint8_t LSN = data[pos+7];
	uint8_t reserved3 = data[pos+8] & 0xE0 >> 5;
	uint16_t PCRPID = (data[pos+8] & 0x1F) << 8 | (data[pos+9] & 0xFF);
	uint8_t reserved4 = data[pos+10] & 0xF0 >> 4;
	uint16_t PIL = ((data[pos+10] & 0x0F) << 8) | (data[pos+11] & 0xFF);
	pos = pos + 12;
//	pos = pos + PIL;
	
	printf("(TS-PMT): TID: 0x%x | SSI: 0x%x | (reserved1: 0x%x) | SL: %u/0x%x | PN: %u/0x%x | (reserved2: 0x%x) | VN: 0x%x | CNI: 0x%x | SN: 0x%x | LSN: 0x%x | (reserved3: 0x%x) | PCRPID: 0x%x | (reserved4: 0x%x) | PIL: %u/0x%x\n",
	TID,SSI,reserved1,SL,SL,PN,PN,reserved2,VN,CNI,SN,LSN,reserved3,PCRPID,reserved4,PIL,PIL);
	if (SL >= 0x3fd){
		printf("(PMT) conflict with specification!! [SL must be < 0x3fd]\n");
		return 184;
	}
	
	if (SL <= PIL){
		printf("(CAT) PIL > SL. Sense?\n");
		return 184;
	}
	
	i = PIL;
	while (i > 0){
		uint8_t oldpos = pos;
		printf(" ");
		pos = descriptor(data,pos);
		//printf("i: %d oldpos: %d pos: %d (pos-oldpos): %d ",i, oldpos, pos, pos-oldpos);
		i = i - (pos - oldpos);
	}	
	
	i = SL - 9 - PIL;
	while (i > 4){
		uint8_t ST = data[pos];
		uint8_t reserved5 = (data[pos+1] & 0xE0) >> 5;
		uint8_t EPID = (data[pos+1] & 0x1F) << 8 | (data[pos+2] & 0xFF);
		uint8_t reserved6 = (data[pos+3] & 0xF0) >> 4;
		uint16_t ESIL = (data[pos+3] & 0x0F) << 8 | (data[pos+4] & 0xFF);
		pos = pos + 5;
		printf(" (TS-PMT): [%u] ST: %u/0x%x | (reserved5: 0x%x) | EPID: 0x%02x | (reserved6: 0x%x) | ESIL: %u/0x%x\n",i,ST,ST,reserved5,EPID,reserved6,ESIL,ESIL);
		j = ESIL;
		while (j > 0){
			uint8_t oldpos = pos;
			printf("  ");
			pos = descriptor(data,pos);
			//printf("i: %d oldpos: %d pos: %d (pos-oldpos): %d ",i, oldpos, pos, pos-oldpos);
			j = j - (pos - oldpos);
		}	
		i = i - ESIL - 5;
	}
	
	
	pos = pos + 4; // CRC32
	return pos;
}