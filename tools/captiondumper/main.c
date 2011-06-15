#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

void bitout_ui8(uint8_t a){
	int i;
	for (i = 0; i < 8; i++){
		printf("%d",(a>>(7-i))&0x1);
	}
}

void usage(char* binary_name){
printf("usage: %s [-pcwv] -i filename\n",binary_name);
printf("switches:\n");
printf("  -i filename - opens \"filename\"\n");
printf("  -o outfile  - writes to \"outfile\"\n");
printf("  -v          - verbose\n");
printf("\n");
}

int main(int argc, char** argv){
	char* filename = NULL;
	char* outfile = NULL;
	FILE *f;
	FILE *of;
	int c;
	int verbose = 0;
	
	while ((c = getopt(argc,argv,"o:i:")) != -1){
		switch (c){
			case 'i':
				filename = optarg;
				if (verbose > 0)
					printf("set in filename to: %s\n",filename);
				break;
			case 'o':
				outfile = optarg;
				if (verbose > 0)
					printf("set out filename to: %s\n",filename);
				break;
			default:
				printf("\n");
				usage(argv[0]);
				abort();
		}
	}

	if (filename == NULL){
		usage(argv[0]);
		return 1;
	}
	if (outfile == NULL){
		of = stdout;
	} else {
		if (!(of = fopen(outfile,"w"))){
			printf("couldn't open outfile.\n");
			return 3;
		}
	}

	char *outdata = (char*)malloc(2048);
	snprintf(outdata,2048,"<xml><appdata><name>TsAnalyzer-SubExtractor</name><version>GIT</version></appdata><data>");
	fwrite(outdata,strlen(outdata),1,of);
	
	char* data = (char*)malloc(188);
	int chk = 0;

	if (!(f = fopen(filename,"r"))){
		printf("couldn't open in file.\n");
		return 2;
	} 
	
	int oldsize = ftell(f);
	
	//sync to syncbyte
	while (!feof(f) && chk != 0x47){
		chk = fgetc(f);
	}
	
	int newsize = ftell(f);
	if (oldsize != newsize) { fseek(f,-1,SEEK_CUR); oldsize--; }

	if (feof(f)){
		fclose(f);
		printf("EOF before hitting mainloop.\n");
		return 3;
	}
	
	while (!feof(f)){
		uint8_t pos = 0;
		fread(data,188,1,f);
		uint16_t PID = (data[1] & 0x1F) << 8 | (data[2] & 0xff);
		uint8_t PUSI = (data[1] & 0x40) >> 6;
		uint8_t AFC = (data[3] & 0x30) >> 4;
		pos = pos + 4;
		if (AFC == 0x02 || AFC == 0x03){
			pos = pos + data[pos] + 1;
		}
		if (PUSI == 0x01 && (((data[pos] << 16) & 0xFF0000) | ((data[pos+1] << 8) & 0xFF00) | (data[pos+2] & 0xFF)) == 0x000001) {
			uint32_t i;
			uint8_t SID = (data[pos+3]);
			uint16_t orig_PPL = ((data[pos+4] & 0xFF00) << 8) | (data[pos+5] & 0xFF);
			uint16_t PPL = orig_PPL;
			pos = pos + 6;
			if (SID != 0xBD)
				continue;
			
			if (SID != 0x00) {
				uint8_t PTSDTSF  = (data[pos+1] & 0xC0) >> 6;
				uint8_t ESCRF    = (data[pos+1] & 0x20) >> 5;
				uint8_t ESRF     = (data[pos+1] & 0x10) >> 4;
				uint8_t DSMTMF   = (data[pos+1] & 0x08) >> 3;
				uint8_t ACIF     = (data[pos+1] & 0x04) >> 2;
				uint8_t PESCRCF  = (data[pos+1] & 0x02) >> 1;
				uint8_t PESEF    = (data[pos+1] & 0x01);
				uint8_t PESHDL  = data[pos+2];
				
				pos = pos + 3;
				PPL = PPL - 3;
				
				snprintf(outdata,2048,"\n<PES><PID>%02X</PID>",PID);
				if (PTSDTSF == 0x2){
					snprintf(outdata,2048,"\n<PES><PID>%02X</PID><DTS>%02X%02X%02X%02X%02X</DTS>",PID,(uint8_t)data[pos],(uint8_t)data[pos+1],(uint8_t)data[pos+2],(uint8_t)data[pos+3],(uint8_t)data[pos+4]);
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

					uint8_t PESPDF = (data[pos] & 0x80) >> 7;
					uint8_t PHFF = (data[pos] & 0x40) >> 6;
					uint8_t PPSCF = (data[pos] & 0x20) >> 5;
					uint8_t PSTDBF = (data[pos] & 0x10) >> 4;
					uint8_t PESEF2 = data[pos] & 0x01;
					pos++;
					
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

				for (i = 0; PESHDL>0; i++) {pos++; PPL--; PESHDL--; }
				
				uint8_t data_group_id = (data[pos] & 0xFC) >> 2;
				//printf("pos: %d PESHDL %d PPL: %d\n",pos,PESHDL,PPL);
				
				pos = pos + 3;
				PPL = PPL - 3;

				if (data_group_id != 0x20){
					//snprintf(outdata,2048,"<TYPE>INVALID-PRE-DG-TYPE %02X</TYPE></DG></PES>",data_group_id);
					//fwrite(outdata,strlen(outdata),1,of);
					continue;
				}
				
				data_group_id = (data[pos] & 0xFC) >> 2;

				if ( (data_group_id > 0x08  && data_group_id < 0x20) || (data_group_id > 0x28) ){
					//snprintf(outdata,2048,"<TYPE>INVALID-TYPE %02X</TYPE></DG></PES>",data_group_id);
					//fwrite(outdata,strlen(outdata),1,of);
					continue;
				}

				uint16_t data_group_size = ((data[pos+3] << 8) & 0xFF00) | (data[pos+4] & 0xFF);
				
				pos = pos + 5;
				PPL = PPL - 5;

				if (data_group_size > PPL){
					//snprintf(outdata,2048,"<TYPE>PPL-INVALID</TYPE></DG></PES>");
					//fwrite(outdata,strlen(outdata),1,of);
					continue;
				}

				char* closetag = malloc(2*sizeof(char));
				if (data_group_id == 0x0 || data_group_id == 0x20) {
					fwrite(outdata,strlen(outdata),1,of);
					snprintf(outdata,2048,"<DG><TYPE>CAPTION MANAGEMENT</TYPE>");
					fwrite(outdata,strlen(outdata),1,of);
					sprintf(closetag,"CM");
					
					uint8_t TMD = (data[pos] & 0xC0) >> 6;
					uint8_t reserved1 = data[pos] & 0x3F;
					pos++;
					snprintf(outdata,2048,"<CM><TMD>%02X</TMD>",(uint8_t)TMD);
					fwrite(outdata,strlen(outdata),1,of);
					
					//printf("(ARIB-CM): TMD: %X | reserved1: %X | ",TMD,reserved1);
					if (TMD == 0x1 || TMD == 0x02){
						snprintf(outdata,2048,"<OTM>%02X%02X%02X%02X%02X</OTM>",(uint8_t)data[pos+0],(uint8_t)data[pos+1],(uint8_t)data[pos+2],(uint8_t)data[pos+3],(uint8_t)data[pos+4]);
						fwrite(outdata,strlen(outdata),1,of);
						pos=pos+5;
					}
					
					uint8_t numlang = data[pos];
					snprintf(outdata,2048,"<NUMLANG>%02X</NUMLANG>",(uint8_t)data[pos]);
					fwrite(outdata,strlen(outdata),1,of);
					pos++;

					for (i = 0; i < numlang; i++){
						uint8_t DMF = data[pos] & 0x0F;
						pos++;
						if (DMF == 0x0C || DMF == 0x0D || DMF == 0x0E){
							pos++;
						}
						snprintf(outdata,2048,"<LANG>%c%c%c</LANG>",(uint8_t)data[pos],(uint8_t)data[pos+1],(uint8_t)data[pos+2]);
						fwrite(outdata,strlen(outdata),1,of);

						pos = pos + 3;
						pos++;
					}
				} else {
					fwrite(outdata,strlen(outdata),1,of);
					snprintf(outdata,2048,"<DG><TYPE>CAPTION STATEMENT</TYPE>");
					fwrite(outdata,strlen(outdata),1,of);
					sprintf(closetag,"CS");
					
					uint8_t TMD = (data[pos] & 0xC0) >> 6;
					pos++;
					snprintf(outdata,2048,"<CS><TMD>%02X</TMD>",(uint8_t)TMD);
					fwrite(outdata,strlen(outdata),1,of);
					
					if (TMD == 0x1 || TMD == 0x02){
						snprintf(outdata,2048,"<STM>%02X%02X%02X%02X%02X</STM>",(uint8_t)data[pos+0],(uint8_t)data[pos+1],(uint8_t)data[pos+2],(uint8_t)data[pos+3],(uint8_t)data[pos+4]);
						fwrite(outdata,strlen(outdata),1,of);
						pos=pos+5;
					}
				}
				
				uint32_t dull = ((data[pos] << 16) & 0xFF0000) | ((data[pos+1] << 8) & 0xFF00) | (data[pos+2] & 0xFF);
				uint32_t dullcounter = dull;
				
				snprintf(outdata,2048,"<LEN>%02X%02X%02X</LEN>",(uint8_t)data[pos+0],(uint8_t)data[pos+1],(uint8_t)data[pos+2]);
				fwrite(outdata,strlen(outdata),1,of);
				
				pos = pos + 3;
				PPL = PPL - 3;
				
				while (dullcounter != 0){
					uint8_t oldpos = pos;
					uint32_t data_unit_size = (((data[pos+2] & 0x0000FF) << 16) | ((data[pos+3] & 0x00FF) << 8) | ((data[pos+4] & 0xFF) << 0));
					//uint32_t data_unit_size = ((data[pos+2] << 16) & 0x0000FF) | ((data[pos+3] << 8) & 0x00FF) | (data[pos+4] & 0xFF);						
					if (data_unit_size > 188 || data_unit_size > PPL){
						if (dullcounter < data_unit_size)
							dullcounter=0;
						else
							dullcounter = dullcounter - data_unit_size;
						continue;
					}

					memset(outdata,0,(2048));
					snprintf(outdata,2048,"<DU><DUP>%02X</DUP><LEN>%02X%02X%02X|%08X</LEN><DATA>",data[pos+1],data[pos+2],data[pos+3],data[pos+4],data_unit_size);						
					fwrite(outdata,strlen(outdata),1,of);
					pos = pos + 5;
					for (i = 0; i < data_unit_size; i++){
						sprintf(outdata,"%02X",(uint8_t)data[pos]);
						fwrite(outdata,strlen(outdata),1,of);
						pos++;
					}
					snprintf(outdata,2048,"</DATA></DU>");
					fwrite(outdata,strlen(outdata),1,of);
					dullcounter = dullcounter - (pos - oldpos);
				}
				
				snprintf(outdata,2048,"</%s></DG></PES>",closetag);
				fwrite(outdata,strlen(outdata),1,of);				
			}
		}
				
	}
	snprintf(outdata,2048,"\n</data></xml>\n");
	fwrite(outdata,strlen(outdata),1,of);
	fflush(of);
	if (of != stdout)
		fclose(of);
	fclose(f);
	return 0;
}