#ifndef NUMERIC_LIB_H
#define NUMERIC_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

    #include "../../platformlib/include/platform.h"



	PLATFORM_API char* numeric_int_to_string(int value);
	PLATFORM_API double numeric_parse_double(const char* data, int* check_error);
	PLATFORM_API int numeric_parse_int(char* data, int* check_error);


#ifdef __cplusplus
}
#endif

#endif /* NUMERIC_LIB */