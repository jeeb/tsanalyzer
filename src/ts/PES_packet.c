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
	// AAAAAAAA
	// AAAAAAAA
	// AAAAAAAA
	// BBBBBBBB
	// CCCCCCCC
	// CCCCCCCC
	
	uint32_t PSCF = ((data[pos+0] & 0xFF0000) << 16) | ((data[pos+1] & 0xFF00) << 8) | (data[pos+2] & 0xFF);
	if (PSCF != 0x000001){
		printf("(PES): not a PES. start byte != 1. (");
		bitout_ui32(PSCF);
		printf(")\n");
		return 188;
	}
	uint8_t SID = (data[pos+3]);
	uint16_t PPL = ((data[pos+4] & 0xFF00) << 8) | (data[pos+5] & 0xFF);
	pos = pos + 6;
	
	printf("(PES): stream_id: 0x%X/%u | PPL: %u",SID,SID,PPL);
	
	if (SID != PES_STREAMID_program_stream_map 
	 && SID != PES_STREAMID_padding_stream
	 && SID != PES_STREAMID_private_stream_2
	 && SID != PES_STREAMID_ECM_stream
	 && SID != PES_STREAMID_EMM_stream
	 && SID != PES_STREAMID_program_stream_directory
	 && SID != PES_STREAMID_DSMCC_stream
	 && SID != PES_STREAMID_H_222_1_E){
		// 10DDEFGH
		// IIJKLMNO
		// PPPPPPPP
		uint8_t PESSC   = data[pos] & 0x30 >> 4;
		uint8_t PESP    = data[pos] & 0x08 >> 3;
		uint8_t DAI     = data[pos] & 0x04 >> 2;
		uint8_t COPY    = data[pos] & 0x02 >> 1;
		uint8_t OOC     = data[pos] & 0x01;
		uint8_t PTSDTSF  = data[pos+1] & 0xC0 >> 6;
		uint8_t ESCRF    = data[pos+1] & 0x20 >> 5;
		uint8_t ESRF     = data[pos+1] & 0x10 >> 4;
		uint8_t DSMTMF   = data[pos+1] & 0x08 >> 3;
		uint8_t ACIF     = data[pos+1] & 0x04 >> 2;
		uint8_t PESCRCF  = data[pos+1] & 0x02 >> 1;
		uint8_t PESEF    = data[pos+1] & 0x01;
		uint8_t PESHDL  = data[pos+2];
		pos = pos + 3;
		if (PTSDTSF == 0x2){
			pos = pos + 5; // 40 bit
		} 
		if (PTSDTSF == 0x3){
			pos = pos + 10; // 80 bit
		}
		if (ESCRF == 0x1){
			pos = pos + 6; // 48 bit
		}
		if (ESRF == 0x1){
			pos = pos + 3; // 24 bit
		}
		if (DSMTMF == 0x1){
			pos = pos + 1; // 8 bit
		}
		if (ACIF == 0x1){
			pos = pos + 1; // 8 bit
		}
		if (PESCRCF == 0x1){
			pos = pos + 2; // 16 bit
		}
		if (PESEF == 0x1){
			uint8_t PESPDF = data[pos] & 0x80 >> 7;
			uint8_t PHFF = data[pos] & 0x40 >> 6;
			uint8_t PPSCF = data[pos] & 0x20 >> 5;
			uint8_t PSTDBF = data[pos] & 0x10 >> 4;
			uint8_t reserved = data[pos] & 0x0E >> 1;
			uint8_t PESEF2 = data[pos] & 0x01;
			pos++;
			if (PESPDF == 0x01){
				pos = pos + 25;
			}
			if (PHFF == 0x01){
				pos = pos + data[pos] + 1;
			}
			if (PPSCF == 0x01){
				pos = pos + 2;
			}
			if (PSTDBF == 0x01){
				pos = pos + 2;
			}
			if (PESEF2 == 0x01){
				uint8_t PEFL = data[pos] & 0x7F;
				pos = pos + 1 + PEFL;
			}
		}
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