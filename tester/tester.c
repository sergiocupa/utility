#include "stringlib.h"
#include <stdio.h>
#include "string.h"


int main()
{
	const char* test_split1 = "POST /api/usuarios  HTTP/1.1 ";
	const char* test_split2 = " ";
	const char* test_split3 = "";
	int len = strlen(test_split1);

	StringArray* parts = string_split(test_split1, len, " ", 1, true);

	if (parts->Count != 3)
	{
		printf("Split com contagem invalida");
		getchar();
		return 1;
	}

	getchar();
	return 0;
}

