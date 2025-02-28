#include "stringlib.h"
#include <stdio.h>
#include "string.h"


int main()
{
	const char* test_split = "POST /api/usuarios HTTP/1.1";
	int len = strlen(test_split);

	StringArray* parts = string_split(test_split, len, " ", 1);

	if (parts->Count != 3)
	{
		printf("Split com contagem invalida");
		getchar();
		return 1;
	}

	getchar();
	return 0;
}

