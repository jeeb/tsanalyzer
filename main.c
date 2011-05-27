#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SYNC_BYTE 0x47
#define TS_MAXDATA 188

#define VERBOSE 1



#ifdef DEBUG
	#define VERBOSE 1
#endif

void sync_to_syncbyte(FILE *f);
void bitout(int a);
void MPEG_transport_stream(char* data);
int program_association_section(char* data, uint8_t pos);
uint8_t adaptation_field(char* data);

void sync_to_syncbyte(FILE *f){
	int old = ftell(f);
	int chk = 0;
	while (!feof(f) && chk != SYNC_BYTE){
		chk = fgetc(f);
	}
	int new = ftell(f);
	if (old != new) { fseek(f,-1,SEEK_CUR); }
	printf("synced to 0x47. skipped %d byte(s).\n",new-old-1);
}
void bitout(int a){
	int i;
	for (i = 0; i < 8; i++){
		printf("%d",(a>>7-i)&0x1);
	}
}
void bitout_16(uint16_t a){
	int i;
	for (i = 0; i < 16; i++){
		printf("%d",(a>>15-i)&0x1);
	}
}
void MPEG_transport_stream(char* data){
	//GLOBDEC
	int i;

	// ARIB: first 4 bytes = timestamp
	//uint32_t timestamp = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
	
	// Byte 1: 8 bit SYNC BYTE
	// Byte 2: 1 bit TEI, 1 bit TP, 1 bit PUSI, 5 bit PID
	// Byte 3: 8 bit PID
	// Byte 4: 2 bit TSC, 2 bit AFC, 4 bit CC
	
	uint8_t sync_byte = data[0];
	uint8_t TEI = (data[1]      & 0x80) >> 7;
	uint8_t TP = (data[1] 	 & 0x40) >> 6;
	uint8_t PUSI = (data[1]      & 0x20) >> 5;
	uint8_t TSC = (data[3]      & 0xC0) >> 6;
	uint8_t AFC = (data[3]      & 0x30) >> 4;
	uint8_t CC = (data[3]      & 0x0F);
	uint16_t PID = (data[1]      & 0x1F) << 8 | (data[2] && 0xff);

	#ifdef DEBUG
		for (int i = 0; i < 4; i++)
			printf("data[i]: %d / ",data[i]); bitout(data[i]); printf("\n");
	#endif
	
	/*
	if (ARIB) {
		printf("[TS=%u] TEI: 0x%x | TP: 0x%x | PUSI: 0x%x | PID: 0x%4x | TSC: 0x%x | AFC: 0x%x | CC: 0x%x\n",timestamp,TEI,TP,PUSI,PID,TSC,AFC,CC);
	}
	*/
	
	#ifdef VERBOSE
		printf("TEI: 0x%x | TP: 0x%x | PUSI: 0x%x | PID: 0x%x | TSC: 0x%x | AFC: 0x%x | CC: 0x%x\n",TEI,TP,PUSI,PID,TSC,AFC,CC);
	#endif
	
	// starting at data[4].
	uint8_t datapos = 4;
	
	if (AFC == 0x02 || AFC == 0x03){
		#ifdef VERBOSE
			printf("adaption_field detected.\n");
		#endif
		
		datapos = datapos + adaptation_field(data);
		
		#ifdef DEBUG
			printf("datapos @ %d\n",datapos);
		#endif
		
	} else if (AFC == 0x01 || AFC == 0x03){
		while (datapos < 184){
			if (PID == 0x0) {
				#ifdef VERBOSE
					printf("PAT detected.\n");
				#endif
				
				datapos = program_association_section(data,datapos);
				
				#ifdef DEBUG
					printf("datapos @ %d\n",datapos);
				#endif
			} else {
				printf("@PID %u - not implemented yet...\n",PID);
				datapos=184;
			}
			fflush(stdout);
		}
	} else {
		printf("Reserved for future use. What to do now?\n");
	}
}
uint8_t adaptation_field(char* data){
	/* NOT IMPLEMENTED YET. */
	printf("(AF) returning %u.\n",data[4]);
	return data[4];
}
int program_association_section(char* data, uint8_t pos){
	
	// DEFINITION:
	//
	// a = tid, b = ssi, c='0', d=reserved1, e=sl
	// f = tsid , g = reserved2, h = vn, i = cni
	// j = sn, k = lsn, l = pn, m = reserved3, 
	// n = npid OR pmpid, o = crc32
	// 
	// STRUCTURE:
	// AAAAAAAA
	// BCDDEEEE
	// EEEEEEEE
	// FFFFFFFF
	// FFFFFFFF
	// GGHHHHHI
	// JJJJJJJJ
	// KKKKKKKK
	// for cnt = 0; cnt < val(f)-8; cnt++
	//   LLLLLLLL
	//   LLLLLLLL
	//   MMMNNNNN
	//   NNNNNNNN
	// OOOOOOOO
	// OOOOOOOO
	// OOOOOOOO
	// OOOOOOOO
	
	//GLOBDEC
	uint16_t i;
	
	#ifdef DEBUG
		for (i = 0; i < 8; i++){
			printf("%d: ",i);
			bitout(data[pos+i]);
			printf("\n");
		}
	#endif

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
	
	pos = pos + 7;
	
	printf("(PAT) TID: 0x%x | SSI: 0x%x | (reserved1: 0x%x) | SL: 0x%x | TSID: 0x%x | (reserved2: 0x%x) | VN: 0x%x | CNI: 0x%x | SN: 0x%x | LSN: 0x%x\n",TID,SSI,reserved1,SL,TSID,reserved2,VN,CNI,SN,LSN);

	if (TID == 0xFF){
		printf("(PAT) forbidden table_id!\n");
		return 184;
	}

	// 0000XXYYYYYYYYYY
	if (SL & 0xC00 >> 10 != 0){
		printf("(PAT) conflict with specification!! [first 2 bits in SL must be '0']");
		#ifdef DEBUG
			printf(" (");
			bitout_16(SL);
			printf(")");
		#endif
		printf("\n");
		return 184;	
	
	}
	if (SL >= 0x3fd){
		printf("(PAT) conflict with specification!! [SL must be < 0x3fd]\n");
		return 184;
	}
	
	for (i = 0; i < (SL-4)/4; i++){
		uint16_t PN = data[pos] << 8 | (data[pos+1] & 0xff);
		uint8_t reserved3 = data[pos+2] & 0xE0 >> 5;
		uint16_t NPID = 0;
		uint16_t PMPID = 0;
		if (PN == 0){
			NPID = data[pos+2] & 0x1F << 8 | (data[pos+3] & 0xff);
		} else {
			PMPID = data[pos+2] & 0x1F << 8 | (data[pos+3] & 0xff);
		}

		#ifdef DEBUG
			int j;
			for (j = 0; i < 4; i++) 
				printf("%d: ",j); bitout(data[pos+j]);printf("\n");
				
			bitout_16(PN);printf("\n");
		#endif

		pos = pos+4;
	
		#ifdef VERBOSE
			printf("(PAT) [%u]  PN: 0x%x | (reserved3: 0x%x) | NPID: 0x%x | PMPID: 0x%x\n", i, PN, reserved3, NPID, PMPID);
		#endif
	}
	
	pos = pos+4;//CRC_32...
	return pos;
}
int main(int argc, char** argv){
	FILE *f;
	char* data = (char*)malloc(TS_MAXDATA);
	
	if (argc < 2){
		printf("usage: %s filename\n",argv[0]);
		return 1;
	}
	
	f = fopen(argv[1],"r");
	sync_to_syncbyte(f);

	if (feof(f)){
		fclose(f);
		printf("EOF before hitting mainloop.\n");
		return 2;
	}
	
	while (!feof(f)){
		fread(data,TS_MAXDATA,1,f);
		MPEG_transport_stream(data);
		//sync_to_syncbyte(f);
	}
	fclose(f);
	
	return 0;
}
