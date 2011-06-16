#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <iconv.h>
#include "../../src/captions/ARIB/ARIB_tables.h"
#include "x0215_table.h"

#define ARIB_USE_HIRAGANA_TABLE 0x30
#define ARIB_USE_KATAKANA_TABLE 0x31
#define ARIB_USE_ALPHANUMERIC_TABLE 0x4A

int char2hex(char in){
	switch(in){
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'A':
			return 10;
		case 'B':
			return 11;
		case 'C':
			return 12;
		case 'D':
			return 13;
		case 'E':
			return 14;
		case 'F':
			return 15;
	}
}

char *convertX0213toUTF8(char *toConvert){
	char ob[30];
	size_t inlen, outlen;
	char *inptr;
	char *outptr;
	iconv_t iv = NULL;
	outptr = (char *)ob;
	inptr = toConvert;

	memset(ob,0,30);

	inlen = 3;
	outlen = 30;

	iv = iconv_open("UTF-8","Shift_JISX0213");
	if (iv == NULL){
		printf("ERROR opening iconv.\n");
		return NULL;
	}

	iconv(iv,&inptr,&inlen,&outptr,&outlen);

	printf("%X%X -> %s\n",(uint8_t)toConvert[0],(uint8_t)toConvert[1],ob);
	return outptr;
}

int extractParameter(uint8_t *data, int len){
	//char *z = malloc(sizeof(char)*(len+1));
	int i, p;
	//memset(z,0,len+1);
	p = 0;
	for (i = 0; i < len; i++){
		p = p + ((data[i] & 0x0F) * pow(10,len-i-1));
	}
	return p;
}


char* append_text_char(char* textin, char appendum){
	int newlen = strlen(textin)+1+1;
	char *z = malloc(sizeof(char)*(newlen));
	memset(z,0,newlen);
	snprintf(z,newlen,"%s%c",textin,appendum);
	return z;
}

char* append_text_string(char* textin, char* appendum){
	int newlen = strlen(textin)+strlen(appendum)+1;
	char *z = malloc(sizeof(char)*(newlen));
	memset(z,0,newlen);
	snprintf(z,newlen,"%s%s",textin,appendum);
	printf("size: %d, textin: %s, appendum: %s, z: %s\n",newlen,textin,appendum,z);
	return z;
}

uint8_t* append_to_array(uint8_t* oldarr, uint8_t newval, int len){
	uint8_t *newarr;
	int i;
	newarr = malloc(sizeof(uint8_t)*(len+1));
	for (i = 0; i < len; i++)
		newarr[i] = oldarr[i];
	newarr[len] = newval;
	return newarr;
}

int main(int argc, char** argv){
	//uint8_t arr[81] = {0x9B, 0x37, 0x20, 0x53, 0x9B, 0x31, 0x37, 0x30, 0x3B, 0x33, 0x30, 0x20, 0x5F, 0x9B, 0x36, 0x32, 0x30, 0x3B, 0x34, 0x38, 0x30, 0x20, 0x56, 0x1D, 0x61, 0x9B, 0x33, 0x36, 0x3B, 0x33, 0x36, 0x20, 0x57, 0x9B, 0x34, 0x20, 0x58, 0x9B, 0x32, 0x34, 0x20, 0x59, 0x89, 0x87, 0x90, 0x20, 0x44, 0x90, 0x51, 0x9B, 0x33, 0x33, 0x30, 0x3B, 0x35, 0x30, 0x39, 0x20, 0x61, 0x1B, 0x29, 0x4A, 0x0E, 0x28, 0x8A, 0x1B, 0x29, 0x31, 0x0E, 0x49, 0x22, 0x41, 0x63, 0x24, 0x60, 0x89, 0x1B, 0x29, 0x4A, 0x0E, 0x29};
	uint8_t *arr = NULL;
	int arrlen = 0;
	uint8_t g3charset = 0;
	uint8_t g1charset = ARIB_USE_ALPHANUMERIC_TABLE;
	uint8_t g0charset = 0;
	uint8_t ls3active = 0;
	
	if (argc < 2){
		printf("usage: %s \"HEX-STRING\"\n",argv[0]);
		return 1;
	} else {
		unsigned int nv;
		while (sscanf(argv[1],"%X %*s",&nv) > 0){
			printf("read %x\n",nv);
			arr = append_to_array(arr,nv,arrlen);
			arrlen++;
			argv[1] = &(*argv[1]++);
			argv[1] = &(*argv[1]++);
			argv[1] = &(*argv[1]++);
		}
	}
		
	int pos = 0;
	int current_table = 0;
	char *textout = malloc(1);
	textout[0] = 0;
	
	printf("IN> ");
	for (pos = 0; pos < arrlen; pos++){
		printf("%02X ",arr[pos]);
	}
	printf("\n");
	pos = 0;
	while (pos < arrlen){
		//printf("arrlen: %d - pos: %d - char: %02X\n",arrlen,pos,arr[pos]);
		if (arr[pos] == 0x9B){
			pos++;
			int argstart = pos;
			int arglen = 0;
			while (arr[pos] != 0x3B && arr[pos] != 0x20 && (arr[pos] <= 0x52 || arr[pos] >= 0x70)){
				arglen++;
				pos++;
			}
			int P1 = extractParameter(&(arr[argstart]),arglen);
			//printf("argstart: %d - arglen: %d - P1: %d\n",argstart,arglen,P1);
			printf("P1: %d | ",P1);
			
			pos++;
			if (arr[pos-1] == 0x20 && arr[pos] > 0x52 && arr[pos] < 0x70){
				switch(arr[pos])
				{
					case 0x53:
						printf("SWF");
						break;
					case 0x54:
						printf("CCC");
						break;
					case 0x58:
						printf("SHS");
						break;
					case 0x59:
						printf("SVS");
						break;
					case 0x5D:
						printf("GAA");
						break;
					case 0x64:
						printf("MDF");
						break;
					case 0x65:
						printf("CFS");
						break;
					case 0x66:
						printf("XCS");
						break;
					case 0x68:
						printf("PRA");
						break;
					case 0x69:
						printf("ACS");
						break;
					case 0x6E:
						printf("RCS");
						break;
					default:
						printf("unknown: 0x%X",arr[pos]);
						break;
				}
				printf("\n");
				pos++;					
			} else if (arr[pos-1] == 0x9B){
				if (arr[pos] == 0x5B)
					printf("PLD");
				if (arr[pos] == 0x5C)
					printf("PLU");
				printf("\n");
				pos++;
			} else if (arr[pos] == 0x6F){
				printf("SCS");
				printf("\n");
				pos++;
			} else if (arr[pos-1] == 0x20 || arr[pos-1] == 0x3B){
				argstart = pos;
				arglen = 0;
				while (arr[pos] != 0x3B && arr[pos] != 0x20 && (arr[pos] <= 0x52 || arr[pos] >= 0x70)){
					arglen++;
					pos++;
				}
				int P2 = extractParameter(&(arr[argstart]),arglen);
				//printf("argstart: %d - arglen: %d - P2: %d\n",argstart,arglen,P2);
				printf("P2: %d | ",P2);
				
				pos++;
				if (arr[pos-1] == 0x20){
					switch(arr[pos]){
						case 0x61:
							printf("ACPS");
							break;
						case 0x56:
							printf("SDF");
							break;
						case 0x57:
							printf("SSM");
							break;
						case 0x5F:
							printf("SDP");
							break;
						case 0x42:
							printf("GSM");
							break;
						case 0x5E:
							printf("SRC");
							break;
						case 0x63:
							printf("ORN");
							break;
						default:
							printf("unknown: 0x%X",arr[pos]);
							break;
					}
					printf("\n");
					pos++;
				}
			}
		} else if (arr[pos] == 0x83){
			printf("YLF");
			pos++;
			printf("\n"); 
		} else if (arr[pos] == 0x0F){
			printf("LS0");
			pos++;
			current_table = g0charset;
			printf("\n"); 
		} else if (arr[pos] == 0x0E){
			printf("LS1");
			pos++;
			current_table = g1charset;
			printf("\n"); 
		} else if (arr[pos] == 0x00){
			printf("NUL");
			pos++;
			printf("\n"); 
		} else if (arr[pos] == 0x1D){
			printf("SS3");
			pos++;
			printf(" - single shift G3 @ 0x%X",arr[pos]);
			g3charset = arr[pos]-0x31;
			pos++;
			printf("\n"); 
		} else if (arr[pos] == 0x89){
			printf("MSZ");
			pos++;
			printf("\n");
		} else if (arr[pos] == 0x88){
			printf("SSZ");
			pos++;
			printf("\n");
		} else if (arr[pos] == 0x8A){
			printf("NSZ");
			pos++;
			printf("\n");
		} else if (arr[pos] == 0x87){
			printf("WHF");
			pos++;
			printf("\n");
		} else if (arr[pos] == 0x86){
			printf("CNF");
			pos++;
			printf("\n");
		} else if (arr[pos] == 0x90){
			printf("COL | ");
			pos++;
			if (arr[pos] != 0x20){
				printf("color control: 0x%02X",arr[pos]);
				pos++;
			}
			else {
				pos++;
				printf("palette control: 0x%02X",arr[pos]);
				pos++;
			}			
			printf("\n");
		} else if (arr[pos] == 0x09){
			printf("APF");
			pos++;
			printf("\n");
		} else if (arr[pos] == 0x1B){
			printf("ESC");
			/*while (arr[pos] != 0x0E)
				pos++;
			*/
			pos++;
			if (arr[pos] == 0x29){
				printf(" | SWITCH CHARSET | ");
				pos++;
				if (arr[pos] == ARIB_USE_ALPHANUMERIC_TABLE){
					printf(" using alphanumeric table.");
					current_table = ARIB_USE_ALPHANUMERIC_TABLE;
				} else if (arr[pos] == ARIB_USE_KATAKANA_TABLE) {
					printf(" using katakana table.");
					current_table = ARIB_USE_KATAKANA_TABLE;
				} else {
					printf(" using unknown table: 0x%02X",arr[pos]);
				}
				pos++;
			} else if (arr[pos] == 0x6E){
				printf(" | LS3");
				current_table = g3charset;
				//pos++;
			} else {
				printf(" %X",arr[pos]);
				pos++;
				/*while (arr[pos] != 0x0E){
					printf(" %X",arr[pos]);
					pos++;
				}*/
			}
			printf("\n");
			pos++;
		} else {
			if (current_table != 0){
				if (current_table == ARIB_USE_KATAKANA_TABLE){
					uint32_t arrelm = 0x2500 | arr[pos];
					printf("=%X=",arrelm);
					printf("%s",x0215_mapping[arrelm]);
					textout = append_text_string(textout,x0215_mapping[arrelm]);
				}
				else if (current_table == ARIB_USE_ALPHANUMERIC_TABLE){
					printf("%c",alphanumeric_graphical_set[arr[pos]]);
					textout = append_text_char(textout,alphanumeric_graphical_set[arr[pos]]);
				}
				else if (current_table == ARIB_USE_HIRAGANA_TABLE){
					uint32_t arrelm = 0x2400 | arr[pos];
					printf("=%X=",arrelm);
					printf("%s",x0215_mapping[arrelm]);
					textout = append_text_string(textout,x0215_mapping[arrelm]);
				}
			} else {
				uint32_t arrelm = ((arr[pos] << 8) & 0xFF00) | arr[pos+1];
				printf("=%X=",arrelm);
				printf("%s",x0215_mapping[arrelm]);
				textout = append_text_string(textout,x0215_mapping[arrelm]);
				pos++;
			}
			//printf("no table set yet. (0x%02X)\n",arr[pos]);				
			/*if (arr[pos+1] != 0x1B){
				char *buf = malloc(3*sizeof(char));
				strncpy(buf,&(arr[pos]),2);
				buf = convertX0213toUTF8(buf);
				printf("%s",buf);
				pos++;
			}*/
			pos++;
		}
	}
	printf("\n\nText:\n%s\n",textout);
	return 0;
}