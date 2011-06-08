#include "../common.h"
uint8_t program_association_section(char* data, uint8_t pos, struct transport_stream *ts){
	// global declarations
	uint16_t i;
	uint16_t SLcounter; //Section Length Counter
	
	// items
	uint8_t TID = data[pos];
	uint8_t SSI = data[pos+1] & 0x80 >> 7;
	uint8_t reserved1 = data[pos+1] & 0x30 >> 4;
	uint16_t SL = ((data[pos+1] & 0x0F) << 8 | (data[pos+2] & 0xff));
	uint16_t TSID = (data[pos+3] << 8 | (data[pos+4] & 0xff));
	uint8_t reserved2 = data[pos+5] & 0xC0 >> 6;
	uint8_t VN = data[pos+5] & 0x3E >> 1;
	uint8_t CNI = data[pos+5] & 0x01;
	uint8_t SN = data[pos+6];
	uint8_t LSN = data[pos+7];
	
	pos = pos + 8;

	if (ts->serial_number_set && ts->serial_number == SN){
		printf("(PAT) PAT table already exists, overriding would be stupid.\n");
		return pos + SL;
	}
	
	ts->network_pid_set = 0;
	ts->programs = NULL;
	ts->serial_number = SN;
	ts->serial_number_set = 1;
	
	#ifdef VERBOSE
		printf("(PAT) TID: 0x%x | SSI: 0x%x | (reserved1: 0x%x) | SL: 0x%x | TSID: 0x%x | (reserved2: 0x%x) | VN: 0x%x | CNI: 0x%x | SN: 0x%x | LSN: 0x%x\n",TID,SSI,reserved1,SL,TSID,reserved2,VN,CNI,SN,LSN);
	#endif
	
	if (TID == 0xFF){
		printf("(PAT) forbidden table_id! (0xff)\n");
		return 184;
	}
	
	if ((SL & 0xC00) >> 10 != 0){
		printf("(PAT) conflict with specification!! [first 2 bits in SL must be '0']");
		#ifdef DEBUG
			printf(" (");
			bitout_ui16(SL);
			printf(")");
		#endif
		printf("\n");
		return 184;	
	
	}
	if (SL >= 0x3fd){
		printf("(PAT) conflict with specification!! [SL must be < 0x3fd]\n");
		return 184;
	}
	
	SLcounter = SL - 5;
	for (i = SLcounter; i > 4; i=i-4){
		uint16_t PN = data[pos] << 8 | (data[pos+1] & 0xff);
		uint8_t reserved3 = data[pos+2] & 0xE0 >> 5;
		uint16_t NPID = 0;
		uint16_t PMPID = 0;
		
		if (PN == 0){
			NPID = ((data[pos+2] & 0x1F) << 8) | (data[pos+3] & 0xff);
			
			ts->network_pid = NPID;
			ts->network_pid_set = 1;
		} else {
			PMPID = ((data[pos+2] & 0x1F) << 8) | (data[pos+3] & 0xff);
			
			add_program_to_ts(PN,PMPID,ts);
		}
		
		#ifdef DEBUG
			int j;
			for (j = 0; i < 4; i++) {
				printf("%d: ",j); bitout_ui8(data[pos+j]);printf("\n");
			}
			bitout_ui16(PN);printf("\n");
		#endif

		pos = pos+4;
	
		#ifdef VERBOSE
			printf("(PAT) [%u]  PN: 0x%x | (reserved3: 0x%x) | ",i, PN ,reserved3);
			if (PN == 0)
				printf("NPID: 0x%x",NPID);
			else
				printf("PMPID: 0x%x", PMPID);
			printf("\n");
		#endif
	}
	SLcounter = i;
	
	#ifdef DEBUG
		printf("SLcounter: %u\n",SLcounter);
	#endif
	
	uint32_t crc32 = ((data[pos+0] & 0xFF000000) << 24) | ((data[pos+1] & 0xFF0000) << 16) | ((data[pos+2] & 0xFF00) << 8) | (data[pos+3] & 0xFF) ;
	
	#ifdef VERBOSE
		printf("(PAT) CRC32: %u/%x %x %x %x\n",crc32,(data[pos+0]) , (data[pos+1]) , (data[pos+2]) , (data[pos+3]));
	#endif
	
	pos = pos+4; 

	#ifdef DEBUG
		SLcounter = SLcounter - 4;
		printf("SLcounter: %u\n",SLcounter);
	#endif
	
	return pos;
}