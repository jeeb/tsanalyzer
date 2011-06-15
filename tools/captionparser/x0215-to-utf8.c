#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>

int main(){
	char x[3];
	char ob[30];
	size_t a,b;
	iconv_t iv = NULL;
	char *inptr;
	char *outptr;
	outptr = (char *)ob;
	inptr = x;
	a = 2;
	b = 30;
	
	memset(ob,0,30);
	x[0]=0x81;
	x[1]=0x9C;
	x[2]=0x00;

	iv = iconv_open("UTF-8","Shift_JISX0213");
	if (iv == NULL){
		printf("ERROR opening iconv.\n");
	}

	iconv(iv,&inptr,&a,&outptr,&b);

	printf("%X%X -> %s\n",(uint8_t)x[0],(uint8_t)x[1],ob);
	return 0;
}
