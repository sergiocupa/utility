#ifndef EVENTHUB_H
#define EVENTHUB_H

#ifdef __cplusplus
extern "C" {
#endif

    #include "../../platformlib/include/platform.h"



	typedef void(*ErrorFunc) (int code, const char* content);



	PLATFORM_API void error(int code, const char* format, ...);
	PLATFORM_API void set_error_callback(ErrorFunc callback);



#ifdef __cplusplus
}
#endif

#endif /* EVENTHUB */