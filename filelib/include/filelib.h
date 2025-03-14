#ifndef FILE_LIB_H
#define FILE_LIB_H

#ifdef __cplusplus
extern "C" {
#endif
   
#include "../../platformlib/include/platform.h"


PLATFORM_API bool file_exists(const char* path);
PLATFORM_API int  file_write_text(const char* path_file, char* content, int length);
PLATFORM_API int  file_read_text(const char* path_file, char** out, int* out_length);
PLATFORM_API bool file_read_bin(const char* path_file, byte** out, int* out_length);



#ifdef __cplusplus
}
#endif

#endif /* FILE_LIB */