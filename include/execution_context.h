#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H



#include "file_handle.h"

typedef struct {
	FILE_HANDLE *file_handle;
	EXECUTION_AREA *execution_area;
		
}EXECUTION_CONTEXT;




#endif
