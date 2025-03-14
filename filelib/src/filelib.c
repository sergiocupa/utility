#include "../include/filelib.h"
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>


// To-DO: Criar  CRITICAL_SECTION por caminho do arquivo. Por hora sem este recurso
//        Criar recursos de thread no projeto platform

#ifdef PLATFORM_WIN
    #include <windows.h>

   /* CRITICAL_SECTION fileCriticalSection;
	INIT_ONCE initOnce = INIT_ONCE_STATIC_INIT;

	BOOL CALLBACK InitCriticalSectionOnce(PINIT_ONCE InitOnce, PVOID Parameter, PVOID* Context)
	{
		InitializeCriticalSection(&fileCriticalSection);
		return TRUE;
	}*/

#else 

    #include <unistd.h>
    #include <pthread.h>
    #include <sys/stat.h>

    pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif 



bool file_exists(const char* path)
{
	bool result = true;

	#ifdef PLATFORM_WIN

	    //InitOnceExecuteOnce(&initOnce, InitCriticalSectionOnce, NULL, NULL);
		//EnterCriticalSection(&fileCriticalSection);

		DWORD file_attr = GetFileAttributes(path);

		if (file_attr == INVALID_FILE_ATTRIBUTES)
		{
			result = (GetLastError() == ERROR_FILE_NOT_FOUND);
		}

		//LeaveCriticalSection(&fileCriticalSection);
	#else 
		pthread_mutex_lock(&file_mutex);

		struct stat buffer;
		int file_exists = (stat(filename, &buffer) == 0);

		pthread_mutex_unlock(&file_mutex);

		return file_exists;
	#endif 
	return result;
}




int file_write_text(const char* path_file, char* content, int length)
{
	FILE* file;
	errno_t fe = fopen_s(&file, path_file, "w");

	if (fe == 0)
	{
		size_t leng = fwrite(content, 8, length, file);

		fclose(file);
	}
}


bool file_read_bin(const char* path_file, byte** out, int* out_length)
{
	FILE* file;
	errno_t fe = fopen_s(&file, path_file, "rb");

	if (fe == 0)
	{
		fseek(file, 0, SEEK_END);
		long leng = ftell(file);
		fseek(file, 0, SEEK_SET);

		char* o = (char*)malloc(leng);
		size_t bytesRead = fread(o, 1, leng, file);

		if (bytesRead != leng)
		{
			perror("Erro ao ler o arquivo");
			free(o);
			fclose(file);
			return false;
		}

		fclose(file);

		*out_length = leng;
		*out = o;
		return true;
	}
	return false;
}


int file_read_text(const char* path_file, char** out, int* out_length)
{
	FILE* File;
	errno_t fe = fopen_s(&File, path_file, "r");

	if (fe == 0)
	{
		long leng = 0;
		fseek(File, 0, SEEK_END);
		leng = ftell(File);
		fseek(File, 0, SEEK_SET);

		char* o = (char*)malloc(leng + 1);

		long ix = 0;
		int c = 1;
		while (((c = fgetc(File)) != EOF) && ix < leng)
		{
			o[ix] = c;
			ix++;
		}
		o[ix] = 0;

		fclose(File);

		*out_length = ix;
		*out = o;
		return 1;
	}
	return 0;
}
