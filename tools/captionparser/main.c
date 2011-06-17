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

char* append_text_string(char* textin,const char* appendum){
	int newlen = strlen(textin)+strlen(appendum)+1;
	char *z = malloc(sizeof(char)*(newlen));
	memset(z,0,newlen);
	snprintf(z,newlen,"%s%s",textin,appendum);
	#ifdef DEBUG
		printf("size: %d, textin: %s, appendum: %s, z: %s\n",newlen,textin,appendum,z);
	#endif
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
	int pos = 0;
	int current_table = 0;
	int arrlen = 0;
	char *textout = malloc(1);
	char *assout = malloc(1);
	uint8_t *arr = NULL;
	uint8_t g3charset = 0;
	uint8_t g1charset = ARIB_USE_ALPHANUMERIC_TABLE;
	uint8_t g0charset = 0;

	textout[0] = 0;
	assout[0] = 0;
	
	if (argc < 2){
		printf("usage: %s \"HEX-STRING\"\n",argv[0]);
		return 1;
	} else {
		unsigned int nv;
		while (sscanf(argv[1],"%X %*s",&nv) > 0){
			#ifdef DEBUG
				printf("read %x\n",nv);
			#endif
			arr = append_to_array(arr,nv,arrlen);
			arrlen++;
			argv[1] = &(*argv[1]++);
			argv[1] = &(*argv[1]++);
			argv[1] = &(*argv[1]++);
		}
	}
		
	#ifdef VERBOSE
		printf("Input from command line: ");
		for (pos = 0; pos < arrlen; pos++){
			printf("%02X ",arr[pos]);
		}
		printf("\n");
	#endif
	
	pos = 0;
	while (pos < arrlen){
		#ifdef DEBUG
			printf("arrlen: %d - pos: %d - char: %02X\n",arrlen,pos,arr[pos]);
		#endif
		if (arr[pos] == 0x9B){
			#ifdef VERBOSE
				printf("CSI | ");
			#endif
			pos++;
			int argstart = pos;
			int arglen = 0;

			while (arr[pos] != 0x3B && arr[pos] != 0x20 && (arr[pos] <= 0x52 || arr[pos] >= 0x70)){
				arglen++;
				pos++;
			}

			int P1 = extractParameter(&(arr[argstart]),arglen);
			
			#ifdef VERBOSE
				printf("Parameter 1: %d | ",P1);
			#endif
			
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
		} else if (arr[pos] == 0x00){
			pos++;
			#ifdef VERBOSE
				printf("NUL | Null | Skipping\n");
			#endif
		} else if (arr[pos] == 0x0F){
			#ifdef VERBOSE
				printf("LS0 | Lock Shift G0 | Changing Character Set.\n");
			#endif
			pos++;
			current_table = g0charset;
		} else if (arr[pos] == 0x0E){
			#ifdef VERBOSE
				printf("LS1 | Lock Shift G1 | Changing Character Set.\n");
			#endif
			pos++;
			current_table = g1charset;
		} else if (arr[pos] == 0x1D){
			#ifdef VERBOSE
				printf("SS3 | single shift G3 @ 0x%X - 0x31\n",arr[pos+1]);
			#endif
			g3charset = arr[pos+1]-0x31;
			pos = pos + 2;
		} else if (arr[pos] == 0x89){
			#ifdef VERBOSE
				printf("MSZ | Middle Size\n");
			#endif
			assout = append_text_string(assout,"{\\fscx125\\fscy125}");
			pos++;
		} else if (arr[pos] == 0x88){
			#ifdef VERBOSE
				printf("SSZ | Small Size\n");
			#endif
			assout = append_text_string(assout,"{\\fscx75\\fscy75}");
			pos++;
		} else if (arr[pos] == 0x8A){
			#ifdef VERBOSE
				printf("NSZ | Normal Size\n");
			#endif
			assout = append_text_string(assout,"{\\fscx100\\fscy100}");
			pos++;
		} else if (arr[pos] == 0x87){
			#ifdef VERBOSE
				printf("WHF | White Foreground\n");
			#endif
			assout = append_text_string(assout,"{\\1c&HFFFFFF&}");
			pos++;
		} else if (arr[pos] == 0x86){
			#ifdef VERBOSE
				printf("CNF | Cyan Foreground\n");
			#endif
			assout = append_text_string(assout,"{\\1c&H00FFFF&}");
			pos++;
		} else if (arr[pos] == 0x83){
			#ifdef VERBOSE
				printf("YLF | Yellow Foreground\n");
			#endif
			assout = append_text_string(assout,"{\\1c&H00FF00&}");
			pos++;
		} else if (arr[pos] == 0x90){
			#ifdef VERBOSE
				printf("COL | Color Controls | ");
			#endif
			pos++;
			if (arr[pos] != 0x20){
				#ifdef VERBOSE
					printf("color control: 0x%02X | ",arr[pos]);
				#endif
				switch (arr[pos]){
					case 0x48:
						#ifdef VERBOSE
							printf("foreground color -> transparent.");
						#endif
						//assout = append_text_string(assout,"{\1a&HFF&}");
						break;
					case 0x49:
						#ifdef VERBOSE
							printf("foreground color -> half intensity red.");
						#endif
						break;
					case 0x4A:
						#ifdef VERBOSE
							printf("foreground color -> half intensity green.");
						#endif
						break;
					case 0x4B:
						#ifdef VERBOSE
							printf("foreground color -> half intensity yellow.");
						#endif
						break;
					case 0x4C:
						#ifdef VERBOSE
							printf("foreground color -> half intensity blue.");
						#endif
						break;
					case 0x4D:
						#ifdef VERBOSE
							printf("foreground color -> half intensity magenta.");
						#endif
						break;
					case 0x4E:
						#ifdef VERBOSE
							printf("foreground color -> half intensity cyan.");
						#endif
						break;
					case 0x4F:
						#ifdef VERBOSE
							printf("foreground color -> half intensity white.");
						#endif
						break;
					case 0x50:
						#ifdef VERBOSE
							printf("background color -> black.");
						#endif
						break;
					case 0x51:
						#ifdef VERBOSE
							printf("background color -> full intensity red.");
						#endif
						break;
					case 0x52:
						#ifdef VERBOSE
							printf("background color -> full intensity green.");
						#endif
						break;
					case 0x53:
						#ifdef VERBOSE
							printf("background color -> full intensity yellow.");
						#endif
						break;
					case 0x54:
						#ifdef VERBOSE
							printf("background color -> full intensity blue.");
						#endif
						break;
					case 0x55:
						#ifdef VERBOSE
							printf("background color -> full intensity magenta.");
						#endif
						break;
					case 0x56:
						#ifdef VERBOSE
							printf("background color -> full intensity cysn.");
						#endif
						break;
					case 0x57:
						#ifdef VERBOSE
							printf("background color -> full intensity white.");
						#endif
						break;
					case 0x58:
						#ifdef VERBOSE
							printf("background color -> transparent.");
						#endif
						break;
					case 0x59:
						#ifdef VERBOSE
							printf("background color -> half intensity red.");
						#endif
						break;
					case 0x5A:
						#ifdef VERBOSE
							printf("background color -> half intensity green.");
						#endif
						break;
					default:
						printf("i don't care about the color, i can't put it in ass anyways.");
						break;
				}
				pos++;
				printf("\n");
			}
			else {
				pos++;
				#ifdef VERBOSE
					printf("palette number: 0x%d",arr[pos]);
				#endif
				pos++;
			}			
			printf("\n");
		} else if (arr[pos] == 0x09){
			#ifdef VERBOSE
				printf("APF | Active Position Forward\n");
			#endif
			textout = append_text_string(textout," ");
			assout = append_text_string(assout," ");
			pos++;
		} else if (arr[pos] == 0x1B){
			#ifdef VERBOSE
				printf("ESC | Escape Sequence | ");
			#endif
			pos++;
			if (arr[pos] == 0x29){
				#ifdef VERBOSE
					printf("Switching to Charset | ");
				#endif
				pos++;
				if (arr[pos] == ARIB_USE_ALPHANUMERIC_TABLE){
					#ifdef VERBOSE
						printf(" using alphanumeric table.");
					#endif
					current_table = ARIB_USE_ALPHANUMERIC_TABLE;
				} else if (arr[pos] == ARIB_USE_KATAKANA_TABLE) {
					#ifdef VERBOSE
						printf(" using katakana table.");
					#endif
					current_table = ARIB_USE_KATAKANA_TABLE;
				} else {
					printf(" using unknown table: 0x%02X",arr[pos]);
				}
				pos++;
			} else if (arr[pos] == 0x6E){
				#ifdef VERBOSE
					printf("LS3 | Lock Shift to G3");
				#endif
				current_table = g3charset;
				//pos++;
			} else {
				#ifdef VERBOSE
					printf("unknown ESC command: %X",arr[pos]);
				#endif
				//pos++;
			}
			printf("\n");
			pos++;
		} else {
			if (current_table != 0){
				if (current_table == ARIB_USE_KATAKANA_TABLE){
					uint32_t arrelm = 0x2500 | arr[pos];
					#ifdef VERBOSE
						printf("=%X= ",arrelm);
						printf("%s",x0215_mapping[arrelm]);
					#endif
					textout = append_text_string(textout,x0215_mapping[arrelm]);
					assout = append_text_string(assout,x0215_mapping[arrelm]);
				}
				else if (current_table == ARIB_USE_ALPHANUMERIC_TABLE){
					#ifdef VERBOSE
						printf("%c",alphanumeric_graphical_set[arr[pos]]);
					#endif
					textout = append_text_char(textout,alphanumeric_graphical_set[arr[pos]]);
					assout = append_text_char(assout,alphanumeric_graphical_set[arr[pos]]);
				}
				else if (current_table == ARIB_USE_HIRAGANA_TABLE){
					uint32_t arrelm = 0x2400 | arr[pos];
					#ifdef VERBOSE
						printf("=%X= ",arrelm);
						printf("%s",x0215_mapping[arrelm]);
					#endif
					textout = append_text_string(textout,x0215_mapping[arrelm]);
					assout = append_text_string(assout,x0215_mapping[arrelm]);
				}
			} else if ((arr[pos] > 0x1F && arr[pos] < 0x7F) || (arr[pos] > 0xA0 && arr[pos] < 0xFF)) {
				uint32_t arrelm = ((arr[pos] << 8) & 0xFF00) | arr[pos+1];
				#ifdef VERBOSE
					printf("=%X= ",arrelm);
					printf("%s",x0215_mapping[arrelm]);
				#endif
				textout = append_text_string(textout,x0215_mapping[arrelm]);
				assout = append_text_string(assout,x0215_mapping[arrelm]);
				pos++;
			} else {
				printf("unhandled code: 0x%X\n",arr[pos]);
			}
			#ifdef VERBOSE
				printf("\n");
			#endif
			pos++;
		}
	}
	printf("\n\nText:\n%s\n",textout);
	printf("\nASS-line:\n%s\n",assout);
	return 0;
}