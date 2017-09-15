#ifndef EXECUTION_AREA_H
#define EXECUTION_AREA_H

#define COMMIT_CHUNK 0x2000
#include <stdlib.h>



typedef struct {
	void* address_map[256];
}ADDRESS_MAP;

typedef struct {
	void* base_address;
	void* top_address;
	void* commit_address;
	ADDRESS_MAP *address_map;
}EXECUTION_AREA;



EXECUTION_AREA* create_execution_area(void* base_address);
int destroy_execution_area(EXECUTION_AREA *execution_area);
void* allocate_address(EXECUTION_AREA* execution_area, size_t size);


#endif
