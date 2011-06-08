#ifndef _TS_H
#define _TS_H
#include "common.h"

void MPEG_transport_stream(char* filename, struct transport_stream *ts);
void transport_packet(char* data, struct transport_stream *ts);
uint8_t program_association_section(char* data, uint8_t pos, struct transport_stream *ts);
uint8_t adaptation_field(char* data, uint8_t pos);
uint8_t CA_section(char* data, uint8_t pos);
uint8_t CA_descriptor(char* data, uint8_t pos);
uint8_t TS_program_map_section(char* data, uint8_t pos, struct transport_stream *ts);
uint8_t descriptor(char* data, uint8_t datapos);
uint8_t PES_packet(char* data, uint8_t pos);
#endif
