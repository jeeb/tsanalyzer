#ifndef _TSUTILS_H
#define _TSUTILS_H
void sync_to_syncbyte(FILE *f);
int is_PID_PMT(uint16_t program_id, struct transport_stream *ts);
void add_program_to_ts(uint16_t program_id, uint16_t program_map_id, struct transport_stream *ts );
void add_elementary_mapping_to_program(uint16_t program_id, uint8_t stream_type, uint8_t elementary_program_id, struct transport_stream *ts);
#endif