#include "../common.h"
#define PES_STREAMID_program_stream_map 		0xBC
#define PES_STREAMID_private_stream_1  			0xBD
#define PES_STREAMID_padding_stream     		0xBE
#define PES_STREAMID_private_stream_2   		0xBF
#define PES_STREAMID_ECM_stream      		    0xF0
#define PES_STREAMID_EMM_stream     		    0xF1
#define PES_STREAMID_DSMCC_stream   		    0xF2
#define PES_STREAMID_13522_stream    		    0xF3
#define PES_STREAMID_H_222_1_A       		    0xF4
#define PES_STREAMID_H_222_1_B					0xF5
#define PES_STREAMID_H_222_1_C					0xF6
#define PES_STREAMID_H_222_1_D					0xF7
#define PES_STREAMID_H_222_1_E					0xF8
#define PES_STREAMID_ancillary_stream			0xF9
#define PES_STREAMID_SL-packetized_stream 		0xFA
#define PES_STREAMID_FluxMux_stream     		0xFB
#define PES_STREAMID_metadata_stream			0xFC
#define PES_STREAMID_extended_stream_id			0xFD
#define PES_STREAMID_reserved_data_stream 		0xFE
#define PES_STREAMID_program_stream_directory 	0xFF
uint8_t PES_packet(char* data, uint8_t pos){
	//uint32_t PSCF = ((data[pos+0] & 0xFF0000) << 16) | ((data[pos+1] & 0xFF00) << 8) | (data[pos+2] & 0xFF);
	uint32_t PSCF = ((data[pos+0] << 16) & 0xFF0000) | ((data[pos+1]  << 8)& 0xFF00) | (data[pos+2] & 0xFF);
	if (PSCF != 0x000001){
		#ifdef DEBUG
			printf("(PES): not a PES. start byte != 1. (");
			bitout_ui32(PSCF);
			printf(")\n");
		#endif
		return 188;
	}
	
	
	uint8_t i;
	uint8_t SID = (data[pos+3]);
	uint16_t orig_PPL = ((data[pos+4] & 0xFF00) << 8) | (data[pos+5] & 0xFF);
	uint16_t PPL = orig_PPL;
	pos = pos + 6;
	
	#ifdef VERBOSE
		printf("(PES): stream_id: 0x%X/%u | PPL: %u\n",SID,SID,PPL);
	#endif
	
	if (SID != PES_STREAMID_program_stream_map 
	 && SID != PES_STREAMID_padding_stream
	 && SID != PES_STREAMID_private_stream_2
	 && SID != PES_STREAMID_ECM_stream
	 && SID != PES_STREAMID_EMM_stream
	 && SID != PES_STREAMID_program_stream_directory
	 && SID != PES_STREAMID_DSMCC_stream
	 && SID != PES_STREAMID_H_222_1_E){
		uint8_t PESSC   = (data[pos] & 0x30) >> 4;
		uint8_t PESP    = (data[pos] & 0x08) >> 3;
		uint8_t DAI     = (data[pos] & 0x04) >> 2;
		uint8_t COPY    = (data[pos] & 0x02) >> 1;
		uint8_t OOC     = (data[pos] & 0x01);
		uint8_t PTSDTSF  = (data[pos+1] & 0xC0) >> 6;
		uint8_t ESCRF    = (data[pos+1] & 0x20) >> 5;
		uint8_t ESRF     = (data[pos+1] & 0x10) >> 4;
		uint8_t DSMTMF   = (data[pos+1] & 0x08) >> 3;
		uint8_t ACIF     = (data[pos+1] & 0x04) >> 2;
		uint8_t PESCRCF  = (data[pos+1] & 0x02) >> 1;
		uint8_t PESEF    = (data[pos+1] & 0x01);
		uint8_t PESHDL  = data[pos+2];
		#ifdef VERBOSE
			printf("(PES) flags data = "); bitout_ui8(data[pos]); printf(" "); bitout_ui8(data[pos+1]); 
			printf(" | PEESC: 0x%X | PESP: 0x%X | Copyright: 0x%X | PESHDL: 0x%X\n",PESSC,PESP,COPY,PESHDL);
			printf("(PES) flags: PTSDTS 0x%x ESCR 0x%x ESR 0x%x DSMTM 0x%x ACI 0x%x PESCRC 0x%x PESE 0x%x\n",PTSDTSF,ESCRF,ESRF,DSMTMF,ACIF,PESCRCF,PESEF);
		#endif
		
		pos = pos + 3;
		PPL = PPL - 3;
		
		if (PTSDTSF == 0x2){
			#ifdef VERBOSE
				printf("PTSDTS -> "); for (i = 0; i < 5; i++) { bitout_ui8(data[pos+i]); printf(" "); } printf("\n");
			#endif
			pos = pos + 5; // 40 bit
			PPL = PPL - 5;
			PESHDL = PESHDL - 5;
		} 
		if (PTSDTSF == 0x3){
			pos = pos + 10; // 80 bit
			PPL = PPL - 10;
			PESHDL = PESHDL - 10;
		}
		if (ESCRF == 0x1){
			pos = pos + 6; // 48 bit
			PPL = PPL - 6;
			PESHDL = PESHDL - 6;
		}
		if (ESRF == 0x1){
			pos = pos + 3; // 24 bit
			PPL = PPL - 3;
			PESHDL = PESHDL - 3;
		}
		if (DSMTMF == 0x1){
			pos = pos + 1; // 8 bit
			PPL = PPL - 1;
			PESHDL = PESHDL - 1;
		}
		if (ACIF == 0x1){
			pos = pos + 1; // 8 bit
			PPL = PPL - 1;
			PESHDL = PESHDL - 1;
		}
		if (PESCRCF == 0x1){
			pos = pos + 2; // 16 bit
			PPL = PPL - 2;
			PESHDL = PESHDL - 2;
		}
		if (PESEF == 0x1){
			#ifdef VERBOSE
				printf("(PES) PES extension flag set. pos@%d\n",pos);
			#endif

			uint8_t PESPDF = (data[pos] & 0x80) >> 7;
			uint8_t PHFF = (data[pos] & 0x40) >> 6;
			uint8_t PPSCF = (data[pos] & 0x20) >> 5;
			uint8_t PSTDBF = (data[pos] & 0x10) >> 4;
			uint8_t reserved = (data[pos] & 0x0E) >> 1;
			uint8_t PESEF2 = data[pos] & 0x01;
			pos++;

			#ifdef VERBOSE
				printf("(PES)(E) flags: PESPD 0x%X PHF 0x%X PPSC 0x%X PSTDB 0x%X (reserved 0x%X) PESE2 0x%X\n",PESPDF,PHFF,PPSCF,PSTDBF,reserved,PESEF2);
			#endif
			
			PESHDL = PESHDL - 1;
			PPL = PPL - 1;
			
			if (PESPDF == 0x01){
				pos = pos + 16;
				PPL = PPL - 16;
				PESHDL = PESHDL - 16;
			}
			if (PHFF == 0x01){
				PPL = PPL - 1 - data[pos];
				PESHDL = PESHDL - 1 - data[pos];
				pos = pos + data[pos] + 1;
			}
			if (PPSCF == 0x01){
				pos = pos + 2;
				PPL = PPL - 2;
				PESHDL = PESHDL - 2;
			}
			if (PSTDBF == 0x01){
				pos = pos + 2;
				PPL = PPL - 2;
				PESHDL = PESHDL - 2;
			}
			if (PESEF2 == 0x01){
				uint8_t PEFL = data[pos] & 0x7F;
				uint8_t SIDEF = data[pos+1] & 0xF0 >> 7;
				
				#ifdef VERBOSE
					printf("(PES) PES extension 2 flag set. - length: %d\n",PEFL);
				#endif
				
				pos = pos + 2;
				PPL = PPL - 2;
				PESHDL = PESHDL - 2;
				if (SIDEF == 0x0){
					pos = pos + PEFL;
					PPL = PPL - PEFL;
					PESHDL = PESHDL - PEFL;
				}
			}
		}
		#ifdef DEBUG
			printf("pos: %d PESHDL %d PPL: %d\n",pos,PESHDL,PPL);
			for (i = 0; PESHDL>0; i++) {pos++; PPL--; PESHDL--; bitout_ui8(data[pos-1]);}printf("\n"); 
			printf("pos: %d PESHDL %d PPL: %d\n",pos,PESHDL,PPL);
		#endif

		uint8_t data_group_id = (data[pos] & 0xFC) >> 2;
		
		if ( (data_group_id != 0x20) ){
			#ifdef VERBOSE
				printf("invalid data_group_id 0x%2X\n",data_group_id);
			#endif
			return pos;
		}
		
		pos = pos + 3;
		PPL = PPL - 3;

		#ifdef DEBUG
			printf("(payload) dump (length %d)",PPL);
			for (i = 0; PPL-i>0; i++) {
				if (i > 0 && i % 2 == 0) printf(" ");
				if (i % 16 == 0) {printf("\n(payload) %08X: ",i);}
				uint8_t a = data[pos+i];
				printf("%02X",a);
				//pos++; 
				//PPL--; 
				//bitout_ui8(data[pos-1]);
			}
			printf("\n"); 
		#endif

		pos = ARIB_data_group(data,pos,PPL);

		#ifdef DEBUG
			printf("data @ pos %d\n",pos);
			printf("PPL: 0x%X/%d\n",PPL,PPL);
		#endif
	} else if (SID == PES_STREAMID_program_stream_map 
	 && SID == PES_STREAMID_private_stream_2
	 && SID == PES_STREAMID_ECM_stream
	 && SID == PES_STREAMID_EMM_stream
	 && SID == PES_STREAMID_program_stream_directory
	 && SID == PES_STREAMID_DSMCC_stream
	 && SID == PES_STREAMID_H_222_1_E){
		pos = pos + PPL;
	} else if (SID == PES_STREAMID_padding_stream){
		pos = pos + PPL;
	}
	 
	 return pos;
}