#ifndef LOG_MESSAGES_H
#define LOG_MESSAGES_H
#include <stdio.h>


#define ERROR_LOG(...) fprintf(stderr, "ERROR: " __VA_ARGS__)
#define INFO_LOG(...) fprintf(stdout, "INFO: " __VA_ARGS__)
#define WARN_LOG(...) fprintf(stdout, "WARNING: " __VA_ARGS__)

#endif
