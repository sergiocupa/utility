#include "../include/allocator.h"
#include <stdlib.h>


void* mem_allocate(uint size)
{
	void* memo = malloc(size);

	if (!memo)
	{
		error(0, "malloc error");
		return 0;
	}

	return memo;
}


void mem_deallocate(void* m)
{
	free(m);
}

void mem_copy(void* origin, void* dest, uint size)
{

}
void mem_copy_int(int* origin, int* dest, uint size)
{

}
void mem_copy_float(float* origin, float* dest, uint size)
{

}

void mem_set(void* origin, uint size, byte value)
{

}

void  mem_set_int(int* origin, uint size, int value)
{

}

void mem_set_float(float* origin, uint size, float value)
{

}