#include "../common.h"
void transport_packet(char* data){
	//GLOBDEC
	int i;
	// ARIB: first 4 bytes = timestamp
	//uint32_t timestamp = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
	
	// Byte 1: 8 bit SYNC BYTE
	// Byte 2: 1 bit TEI, 1 bit TP, 1 bit PUSI, 5 bit PID
	// Byte 3: 8 bit PID
	// Byte 4: 2 bit TSC, 2 bit AFC, 4 bit CC
	
	uint8_t sync_byte = data[0];
	uint8_t TEI = (data[1] & 0x80) >> 7;
	uint8_t PUSI = (data[1] & 0x40) >> 6;
	uint8_t TP = (data[1] & 0x20) >> 5;
	uint16_t PID = ((data[1] & 0x1F) << 8) | (data[2] & 0xff);
	uint8_t TSC = (data[3] & 0xC0) >> 6;
	uint8_t AFC = (data[3] & 0x30) >> 4;
	uint8_t CC = (data[3] & 0x0F);
	#ifdef DEBUG
		for (i = 0; i < 4; i++) {
			printf("data[%d]: %d / ",i,data[i]); bitout_ui8(data[i]); printf("\n");
		}
	#endif
	
//	printf("PID: "); bitout_ui8(data[1] & 0x1F); bitout_ui8(data[2] & 0xff); printf(" "); bitout_ui16(((data[1] & 0x1F) << 8) | (data[2] & 0xff)); printf("\n");
	/*
	if (ARIB) {
		printf("[TS=%u] TEI: 0x%x | TP: 0x%01x | PUSI: 0x%01x | PID: 0x%04x | TSC: 0x%01x | AFC: 0x%01x | CC: 0x%01x\n",timestamp,TEI,TP,PUSI,PID,TSC,AFC,CC);
	}
	*/
	
	#ifdef VERBOSE
		printf("TEI: 0x%x | TP: 0x%x | PUSI: 0x%x | PID: 0x%04x | TSC: 0x%x | AFC: 0x%x | CC: 0x%x",TEI,TP,PUSI,PID,TSC,AFC,CC);
	#endif
	
	// starting at data[4].
	uint8_t datapos = 4;
	
	if (AFC == 0x02 || AFC == 0x03){
		#ifdef VERBOSE
			printf("adaption_field detected.\n");
		#endif
		
		datapos = adaptation_field(data,datapos);
		
		#ifdef DEBUG
			printf("datapos @ %d\n",datapos);
		#endif
		
	} else if (AFC == 0x01 || AFC == 0x03){
		//payload unit start indicator means there is going to be information for us (PAT or PSI)
		if (PUSI == 1){
			printf("\nPUSI => pointer: %d/%x\n",data[datapos],data[datapos]);
			datapos = datapos + 1 + data[datapos];
			//printf("Table ID: 0x%X\n",data[datapos]);
			if (PID == 0x0) {
				#ifdef VERBOSE
					printf("PAT detected.\n");
				#endif
				datapos = program_association_section(data,datapos);
				
				#ifdef DEBUG
					printf("datapos @ %d\n",datapos);
				#endif
			} /*else if (PID == 0x1) {
				#ifdef VERBOSE
					printf("CAT detected.\n");
				#endif
				datapos = CA_section(data,datapos);
				
				#ifdef DEBUG
					printf("datapos @ %d\n",datapos);
				#endif
			}*/else if (PID == 0x101 && data[datapos] == 0 && data[datapos+1] == 1){
					printf("PES detected.\n");
					datapos = PES_packet(data,datapos-1);
			} else if (PID == 0x101 && data[datapos] == 0x02) {
				#ifdef VERBOSE
					printf("PMT detected.\n");
				#endif
				datapos = TS_program_map_section(data,datapos);
				#ifdef DEBUG
					printf("datapos @ %d\n",datapos);
				#endif
			
			} else {
				printf("@PID %u - not implemented yet...\n",PID);
				datapos = 184;
			}
			fflush(stdout);
		}
		else {
			while (datapos < 184){
				printf(" (data)\n");
				datapos = 184;
			}
		}
	} else {
		printf("Reserved for future use. What to do now?\n");
	}
}