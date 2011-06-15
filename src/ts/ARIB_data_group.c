#include "../common.h"

uint8_t ARIB_data_group(char* data, uint8_t pos, uint8_t length){
	uint8_t i;
	printf("(ARIB-DG): data = "); for (i=0; i < 5; i++) {bitout_ui8(data[pos+i]); printf("/0x%2X ",data[pos+i]); } printf("\n");

	uint8_t data_group_id = (data[pos] & 0xFC) >> 2;
	uint8_t data_group_version = data[pos] & 0x03;
	uint8_t data_group_link_number = data[pos+1];
	uint8_t last_data_group_link_number = data[pos+2];
	uint16_t data_group_size = ((data[pos+3] << 8) & 0xFF00) | (data[pos+4] & 0xFF);
	pos=pos+5;
	length = length - 5;
	
	printf("(ARIB-DG): DGID: 0x%X | DGV: 0x%X | DGLN: 0x%X | LDGLN: 0x%X | DGS: 0x%X/%d\n",data_group_id,data_group_version,data_group_link_number,last_data_group_link_number,data_group_size,data_group_size);
	if ( (data_group_id > 0x08  && data_group_id < 0x20) || (data_group_id > 0x28) ){
		printf("invalid data_group_id 0x%2X\n",data_group_id);
		return pos;
	}
	if (data_group_size > length){
		printf("data_group_size > length, bailing out.\n");
		return pos;
	}
	printf("DG: [ ");
	for (i = 0; i < data_group_size && i < length; i++){ uint8_t a = data[pos+i]; printf("%02X",a); if (i > 0 && i % 2 == 0) printf(" "); if (i > 0 && i % 16 == 0) printf("\n"); }
	printf("]\n");
	
	if (data_group_id == 0x0 || data_group_id == 0x20) {
		printf("->Caption management\n");
		pos = ARIB_caption_management(data,pos);
	} else {
		printf("->Caption statement\n");
		pos = ARIB_caption_statement(data,pos);
	}
	
	return pos;
	//pos = pos + data_group_size;
}