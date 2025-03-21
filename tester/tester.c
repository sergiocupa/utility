#include "stringlib.h"
#include "filelib.h"
#include <stdio.h>
#include <string.h>
#include <shalib.h>


int main()
{
	const char* file = "E:\\git\\appserver\\x64\\Debug\\api\\service\\img\\solda.jpg";

	bool exist = file_exists(file);

	String s1 = { 3,3,"api"};
	String s2 = { 3,3,"apa" };

	int eq = string_equals_s(&s1,&s2);


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


	const char* TEST_HEADER = "Content-Type: application/json";

	if (string_equals_char_range(TEST_HEADER, "Content-", 0, 8))
	{
		if (string_equals_char_range(TEST_HEADER, "Type", 8, 4))
		{
			int h = 99;
		}
	}

	const char* utf8_str = "输";

	size_t leng = 0;
	byte* data = string_utf8_to_bytes(utf8_str, &leng);

	
	int leee = strlen(utf8_str);


	const char* ABC = "Sergio";
	char* cc = string_base64_encode(ABC, strlen(ABC));
	int outl = 0;
	byte* bb = string_base64_decode(cc,strlen(cc),&outl);

	

	unsigned char digest[SHA1_BLOCK_SIZE];
	sha1((unsigned char*)ABC, strlen(ABC), digest);

	getchar();
	return 0;
}

