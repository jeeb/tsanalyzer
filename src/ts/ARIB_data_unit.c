#include "../common.h"

uint8_t ARIB_data_unit(char* data, uint8_t pos, uint32_t dull){

	//printf("%02X%02X%02X%02X%02X ",data[pos],data[pos+1],data[pos+2],data[pos+3],data[pos+4]);
	uint8_t unit_seperator = data[pos];
	uint8_t data_unit_parameter = data[pos+1];
	uint32_t data_unit_size = (((data[pos+2] & 0x0000FF) << 16) | ((data[pos+3] & 0x00FF) << 8) | ((data[pos+4] & 0xFF) << 0));
	uint32_t i;
	
	pos = pos + 5;
	printf(">ARIB-DU: US: %X/%c | DUP: %X | DUS: %X | data: ",unit_seperator,unit_seperator,data_unit_parameter,data_unit_size);
	if (data_unit_size > dull){
		printf("\ndata_unit_size > dull. backing the fuck out.\n");
		return pos;
	}
	for (i = 0; i < data_unit_size; i++){
		uint8_t a = data[pos];
		printf("%02X ",a);
		pos++;
		//bitout_ui8(data[pos]);
	}
	printf("\n");
	return pos;
}