#ifndef ALLOCATOR_LIB_H
#define ALLOCATOR_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

    #include "../../platformlib/include/platform.h"


	PLATFORM_API void* mem_allocate(uint size);
	PLATFORM_API void  mem_deallocate(void* m);
	PLATFORM_API void  mem_copy(void* origin, void* dest, uint size);
	PLATFORM_API void  mem_copy_int(int* origin, int* dest, uint size);
	PLATFORM_API void  mem_copy_float(float* origin, float* dest, uint size);
	PLATFORM_API void  mem_set(void* origin, uint size, byte value);
	PLATFORM_API void  mem_set_int(int* origin, uint size, int value);
	PLATFORM_API void  mem_set_float(float* origin, uint size, float value);



#ifdef __cplusplus
}
#endif

#endif /* ALLOCATOR_LIB */