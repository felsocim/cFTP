#include "../include/common.h"

// Displays usage or help message (based on exit code nature) and terminates
// the application with given exit code
void usage(const char * arg_0, const int exit_code) {
	const char * app_name = strrchr(arg_0, '/') + 1;
	if(exit_code != EXIT_SUCCESS) {
		fprintf(stderr, usage_message, app_name);
    } else {
        fprintf(stdout, help_message, app_name);
    }
    exit(exit_code);
}