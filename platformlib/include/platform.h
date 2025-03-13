#ifndef PLATFORM_LIB_H
#define PLATFORM_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdint.h>

	#ifdef _WIN32 || _WIN64
	    #define PLATFORM_WIN
	#endif 


	#ifdef PLATFORM_WIN
	    #define PLATFORM_API __declspec ( dllexport )
	#else 
	    #define PLATFORM_API
	#endif 


    #define false 0
    #define true  1


	typedef int          bool;
	typedef uint_fast8_t byte;
	typedef unsigned int uint;


	void platform_init();


	#ifdef PLATFORM_WIN

        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
        #include <stdio.h>

        #pragma section(".CRT$XCU", read)
	    __declspec(allocate(".CRT$XCU")) static void (*init_ptr)() = platform_init;

	#else 

        #include <stdio.h>

		__attribute__((constructor)) void my_init() 
		{
			platform_init();
		}

	#endif


	


#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_LIB */