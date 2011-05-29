#include "../common.h"

uint8_t CA_descriptor(char* data, uint8_t pos){
	// AAAAAAAA
	// BBBBBBBB
	// CCCCCCCC
	// CCCCCCCC
	// DDDEEEEE
	// EEEEEEEE
	pos = pos + data[pos+1];
	return pos;
}