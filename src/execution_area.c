#include "execution_area.h"
#include "log_messages.h"

#include <stdlib.h>
#include <sys/mman.h>


EXECUTION_AREA* create_execution_area(void* base_address){
	EXECUTION_AREA *execution_area = malloc(sizeof(*execution_area));
	if(execution_area == NULL){
		ERROR_LOG("Malloc in create_execution_area returned NULL\n");
		return NULL;
	}
	execution_area->base_address = base_address;
	execution_area->top_address = base_address;
	execution_area->commit_address = base_address;
	execution_area->address_map = malloc(sizeof (ADDRESS_MAP));
	if (execution_area->address_map == NULL) {
		ERROR_LOG("Address map creation failed\n");
		return NULL;	
	}
	int i;
	for (i = 0; i< 256; i++)
		execution_area->address_map->address_map[i] = NULL;
	return execution_area;
}


int destroy_execution_area(EXECUTION_AREA *execution_area) {
	if(execution_area == NULL)
		return -1;	
	/* Code to deallocate execution area */
	return 0;
}

void* allocate_address(EXECUTION_AREA *execution_area, size_t size) {
	void *allocation = execution_area->top_address;
	void* new_top = (char*)allocation + size;
	void* new_top_aligned = (void*)((((unsigned long long)new_top + COMMIT_CHUNK -1)/COMMIT_CHUNK)*COMMIT_CHUNK);
	if (new_top_aligned > execution_area->commit_address) {
		if(execution_area->commit_address != mmap(execution_area->commit_address, (size_t)new_top_aligned - (size_t)execution_area->commit_address, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)){
			ERROR_LOG("Execution area expansion failed\n");
			return NULL;
		}
		execution_area->commit_address = new_top_aligned;
	}
	execution_area->top_address = new_top;
	return allocation;
}
