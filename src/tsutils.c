#include "common.h"

void sync_to_syncbyte(FILE *f){
	int oldsize = ftell(f);

	int chk = 0;
	while (!feof(f) && chk != SYNC_BYTE){
		chk = fgetc(f);
	}

	int newsize = ftell(f);
	
	if (oldsize != newsize) { fseek(f,-1,SEEK_CUR); oldsize--; }
	
	#ifdef DEBUG
		printf("synced to 0x47. skipped %d byte(s).\n",newsize-oldsize);
	#endif
}

int is_PID_PMT(uint16_t program_id, struct transport_stream *ts){
	if (ts == NULL) return 0;
	if (ts->programs == NULL) return 0;
	
	struct list_element* p = ts->programs;
	while (p != NULL) {
		if (p->element_data != NULL){
			struct ts_program *tsp = (struct ts_program*)(p->element_data);
			#ifdef DEBUG
				printf("is_PID_PMT: 0x%X v.s. 0x%X\n",program_id,tsp->program_map_pid);
			#endif
			if (tsp->program_map_pid == program_id)
				return 1;
		}
		p=p->next;
	}
	return 0;
}

void add_program_to_ts(uint16_t program_id, uint16_t program_map_id, struct transport_stream *ts ){
	if (ts == NULL) return;
	struct list_element *p = ts->programs;
	struct list_element *q = malloc(sizeof(struct list_element));
	struct ts_program *tsp = malloc(sizeof(struct ts_program));
	tsp->program_number = program_id;
	tsp->program_map_pid = program_map_id;
	tsp->elementary_streams = NULL;
	ts->programs = q;
	q->next = p;
	q->element_data = tsp;
}

void add_elementary_mapping_to_program(uint16_t program_id, uint8_t stream_type, uint8_t elementary_program_id, struct transport_stream *ts){
	if (ts == NULL) return;
	if (ts->programs == NULL) return;
	struct list_element *p = ts->programs;
	while (p != NULL) {
		if (p->element_data != NULL){
			struct ts_program *tsp = (struct ts_program*)(p->element_data);
			#ifdef DEBUG
				printf("add_elementary_mapping_to_program: 0x%X v.s. 0x%X\n",program_id,tsp->program_map_pid);
			#endif
			if (tsp->program_map_pid == program_id){
				// create TS Stream
				struct ts_stream *tss = malloc(sizeof(struct ts_stream));
				tss->stream_type = stream_type;
				tss->stream_id = elementary_program_id;
				
				// Add to front of list.
				struct list_element *l;
				l = malloc(sizeof(struct list_element));
				l->element_data = tss;
				l->next = tsp->elementary_streams;
				tsp->elementary_streams = l;
				
				return;
/*				struct list_element *q = tsp->elementary_streams;
				if (q != NULL){
					while (q->next != NULL){
						q = q->next;
					}
				} else {
				
				}*/
			}
		}
		p=p->next;
	}
	return;
	
}