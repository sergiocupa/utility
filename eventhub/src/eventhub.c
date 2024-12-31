#include "../include/eventhub.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>


ErrorFunc ErrorcCallback = 0;


void set_error_callback(ErrorFunc callback)
{
	ErrorcCallback = callback;
}


void error(int code, const char* format, ...)
{
	va_list ap;
	char* fstr = NULL;
	va_start(ap, format);
	int len = vsnprintf(NULL, 0, format, ap);
	va_end(ap);
	fstr = (char*)malloc(len + 1);
	va_start(ap, format);
	if (fstr) vsnprintf(fstr, len + 1, format, ap);
	va_end(ap);

	fprintf(stderr, fstr);
	//perror(fstr); para descrever erro padrao do sistema operacional

	if (ErrorcCallback)
	{
		ErrorcCallback(code, fstr);
		free(fstr);
	}
	else
	{
		free(fstr);
		//exit(EXIT_FAILURE);
		assert(0);
	}
}