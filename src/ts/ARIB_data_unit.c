#include "../common.h"

uint8_t ARIB_data_unit(char* data, uint8_t pos, uint32_t dull){
	uint8_t unit_seperator = data[pos];
	uint8_t data_unit_parameter = data[pos+1];
	uint32_t data_unit_size = (((data[pos+2] & 0x0000FF) << 16) | ((data[pos+3] & 0x00FF) << 8) | ((data[pos+4] & 0xFF) << 0));
	
	pos = pos + 5;
	
	#ifdef VERBOSE
		printf(">ARIB-DU: US: %X/%c | DUP: %X | DUS: %X\n",unit_seperator,unit_seperator,data_unit_parameter,data_unit_size);
	#endif
	
	if (data_unit_size > dull){
		#ifdef DEBUG
			printf("\ndata_unit_size > dull. backing the fuck out.\n");
		#endif
		return pos;
	}
	
	#ifdef DEBUG
		uint32_t i;
		printf(">ARIB-DU: data: ");
		char *out_text = malloc(sizeof(char)*(data_unit_size+1));
		memset(out_text,0,(data_unit_size+1));
		for (i = 0; i < data_unit_size; i++){
			uint8_t a = data[pos];
			printf("%02X",a);
			if (data[pos] < 0x20 || data[pos]==0x9B) out_text[i] = "."; 
			else out_text[i] = data[pos];
			pos++;
			
		}
		printf (" [%s]",out_text);
	#endif
	
	ARIB_parse_caption_payload(data,pos,data_unit_size);
	pos = pos + data_unit_size;


	return pos;
}