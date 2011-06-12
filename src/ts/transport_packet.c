#include "../common.h"
void transport_packet(char* data, struct transport_stream *ts){
	// global declarations
	int i;
	
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
		printf("PID: "); bitout_ui8(data[1] & 0x1F); bitout_ui8(data[2] & 0xff); printf(" "); bitout_ui16(((data[1] & 0x1F) << 8) | (data[2] & 0xff)); printf("\n");
	#endif
	
	#ifdef VERBOSE
		printf("TEI: 0x%X | TP: 0x%X | PUSI: 0x%X | PID: 0x%04x | TSC: 0x%X | AFC: 0x%X | CC: 0x%X",TEI,TP,PUSI,PID,TSC,AFC,CC);
	#endif
	
	// starting at data[4].
	uint8_t datapos = 4;
	
	if (AFC == 0x02 || AFC == 0x03){
		#ifdef VERBOSE
			printf("\nadaption_field detected. - ");
		#endif
		
		datapos = adaptation_field(data,datapos);
		
		#ifdef DEBUG
			printf("datapos @ %d\n",datapos);
		#endif
		
	}
	
	if (AFC == 0x01 || AFC == 0x03) {
		//payload unit start indicator means there is going to be information for us (PAT or PSI)
		if (PUSI == 1){
			#ifdef VERBOSE
				printf("\n");
			#endif
			#ifdef DEBUG
				printf("PUSI => pointer: %d/%X\n",data[datapos],data[datapos]);
			#endif
			
			datapos = datapos + 1 + data[datapos];
			
			if (PID == 0x0) {			//PID 0x00 is reserved for PAT
			
				#ifdef VERBOSE
					printf("PAT detected.\n");
				#endif
				
				datapos = program_association_section(data,datapos,ts);
				
				#ifdef DEBUG
					printf("datapos @ %d\n",datapos);
				#endif
			} else if (PID == 0x1) {	// PID 0x01 is reserved for CAT
				#ifdef VERBOSE
					printf("CAT detected.\n");
				#endif
				
				datapos = CA_section(data,datapos);
				
				#ifdef DEBUG
					printf("datapos @ %d\n",datapos);
				#endif
			} else if (ts->network_pid_set && ts->network_pid == PID){
				#ifdef VERBOSE
					printf("NAT detected.\n");
				#endif
				
				#ifdef DEBUG
					printf("Ignoring network informations. They are network specific.\n"); 
				#endif
			} else if (is_PID_PMT(PID,ts)) {
				#ifdef VERBOSE
					printf("PMT detected.\n");
				#endif
				
				datapos = TS_program_map_section(data,datapos,ts);
				
				#ifdef DEBUG
					printf("datapos @ %d\n",datapos);
				#endif
			
			} else {
				datapos--;
				if ((((data[datapos] & 0xFF0000) << 16) | ((data[datapos+1] & 0xFF00) << 8) | (data[datapos+2] & 0xFF)) == 0x000001) {
					#ifdef VERBOSE
						printf("PES found.\n");
					#endif
					
					datapos = PES_packet(data,datapos);
					
					#ifdef VERBOSE
						printf("PES over.\n");
					#endif
				} else {
					printf("@PID %u - not implemented yet...\n",PID);
					datapos = 184;
				}
			}
			fflush(stdout);
		}
		while (datapos < 184){
			printf(" (data)\n");
			datapos = 184;
		}
	} 
	
	if (AFC == 0) {
		printf("AFC indicates \"Reserved for future use.\" Skipping.\n");
	}
}