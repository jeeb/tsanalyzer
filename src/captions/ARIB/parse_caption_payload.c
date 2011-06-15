#include "../../common.h"

#define C1_CSI 0x9B
#define C0_ESC 0x1B
#define C0_LS1 0x0E
#define C0_LS0 0x0F
#define ARIB_PLD 0x5B
#define ARIB_PLU 0x5C
#define C1_NSZ 0x8A // Normal Size 

int ARIB_parse_caption_payload(char* data, uint8_t pos, uint8_t length){
	char *text = malloc(sizeof(char)*(length+1));
	char *controls = malloc(sizeof(char)*(1024));
	uint8_t i;
	
	while(i < length){
		if (data[pos+i] == C1_CSI){
			printf(">ARIB-PCP: detected CSI\n");
			i++;
			if (data[pos+i] == ARIB_PLD || data[pos+i] == ARIB_PLU){
				printf("unimplemented control sequence\n");
			} else if ((data[pos+i] & 0xF0) == 0x30) {
				uint8_t P1 = data[pos+i] & 0x0F;
				i++;
				printf("intresting, P1=0x%02X\n",P1);
				if ((P1 & 0xF) < 0xA){
					uint8_t I1 = data[pos+i];
					i++;
					if (I1 == 0x20){
						if (data[pos+i] == 0x53){
							printf("found SWF. type = 0x%X\n",P1);
							i++;
						} else if (data[pos+i] == 0x54){
							printf("found CCC. type = 0x%X\n",P1);
							i++;
						} else if (data[pos+i] == 0x6E){
							printf("found RCS. type = 0x%X\n",P1);
							i++;
						}
					}
				}
			} else {
				printf("unknown control sequence\n");
			}
		} else if (data[pos+i] == C0_ESC){
			printf("ESC sequence.\n");
			i++;
			i++;
			i++;
		} else {
			printf("%c",data[pos+i]);
			i++;
		}
	}
	return pos+i;
}