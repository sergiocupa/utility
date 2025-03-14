#include "../include/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


bool platform_initialized = false;


void test_utf8()
{
	const char* utf8_str = "你";
	int leng = strlen(utf8_str);

	if (leng != 3)
	{
		perror("UTF-8 encoding not supported");
		exit(EXIT_FAILURE);
	}
}


void platform_init()
{
	if (platform_initialized) return;

	test_utf8();

	platform_initialized = true;
}