#ifndef LOG_MESSAGES_H
#define LOG_MESSAGES_H
#include <stdio.h>

#define LOG_ALL 0

#if LOG_ALL
	#define ERROR_LOG(...) fprintf(stderr, "ERROR: " __VA_ARGS__)
	#define INFO_LOG(...) fprintf(stdout, "INFO: " __VA_ARGS__)
	#define WARN_LOG(...) fprintf(stdout, "WARNING: " __VA_ARGS__)
#else
	#define ERROR_LOG(...) fprintf(stderr, "ERROR: " __VA_ARGS__)
	#define INFO_LOG(...)
	#define WARN_LOG(...)
#endif
#endif
