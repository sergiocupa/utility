#include "../include/stringlib.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>


#define INITIAL_STRING_LENGTH       200
#define INITIAL_STRING_ARRAY_LENGTH 100

static const char BASE64_TABLE[]     = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const int  BASE64_MOD_TABLE[] = { 0, 2, 1 };


void string_init(String* ni)
{
	ni->Length = 0;
	ni->MaxLength = INITIAL_STRING_LENGTH;
	ni->Data = (char*)malloc((ni->MaxLength + 1) * sizeof(char));
	ni->Data[0] = 0;
}

// (max_length < 0) utiliza valor default
void string_init_length(String* ni, int max_length)
{
	ni->Length = 0;
	ni->MaxLength = max_length >= 0 ? max_length : INITIAL_STRING_LENGTH;
	ni->Data = (char*)malloc((ni->MaxLength + 1) * sizeof(char));
	ni->Data[0] = 0;
}
void string_resize(String* _this, int max_length)
{
	if ((_this->Length + max_length + 1) >= _this->MaxLength)
	{
		_this->MaxLength = ((_this->Length + max_length + 1) + _this->MaxLength) * 2;
		char* content = (char*)realloc(_this->Data, _this->MaxLength * sizeof(char));

		if (!content) assert(0);

		_this->Data = content;
	}
}

String* string_new()
{
	String* ar = (String*)malloc(sizeof(String));
	ar->Length = 0;
	ar->MaxLength = INITIAL_STRING_LENGTH;
	ar->Data = (char*)malloc((ar->MaxLength + 1) * sizeof(char));
	ar->Data[0] = 0;
	return ar;
}

String* string_copy(String* _this)
{
	String* ar = (String*)malloc(sizeof(String));

	ar->Length    = _this->Length;
	ar->MaxLength = _this->Length;
	ar->Data      = (char*)malloc((ar->MaxLength + 1) * sizeof(char));
	ar->Data[0] = 0;

	int ix = 0;
	while (ix < ar->Length)
	{
		ar->Data[ix] = _this->Data[ix];
		ix++;
	}
	ar[ix].Data[ix] = 0;
	return ar;
}

void string_init_copy(String* _this, char* data, int length)
{
	_this->Length = 0;
	_this->MaxLength = INITIAL_STRING_LENGTH;
	_this->Data = (char*)malloc((_this->MaxLength + 1) * sizeof(char));
	int ix = 0;
	while (ix < length)
	{
		_this->Data[ix] = data[ix];
		ix++;
	}
	_this->Data[ix] = 0;
}

String* string_new_length(int max_length)
{
	String* ar = (String*)malloc(sizeof(String));
	ar->Length = 0;
	ar->MaxLength = max_length >= 0 ? max_length : INITIAL_STRING_LENGTH;
	ar->Data = (char*)malloc((ar->MaxLength + 1) * sizeof(char));
	ar->Data[0] = 0;
	return ar;
}

void string_release_data(String* ar)
{
	if (ar && ar->MaxLength > 0)
	{
		free(ar->Data);
	}
	ar->MaxLength = 0;
	ar->Length    = 0;
}
void string_release(String* ar)
{
	if (ar)
	{
		free(ar->Data);
		free(ar);
	}
}

void string_populate(String* source, String* dest)
{
	dest->Data      = source->Data;
	dest->Length    = source->Length;
	dest->MaxLength = source->MaxLength;
}


void string_array_init(StringArray* ar)
{
	ar->Items = (String**)malloc(sizeof(String*) * INITIAL_STRING_ARRAY_LENGTH);
	ar->Count = 0;
	ar->MaxCount = INITIAL_STRING_ARRAY_LENGTH;
	return ar;
}

StringArray* string_array_new()
{
	StringArray* ar = (StringArray*)malloc(sizeof(StringArray));
	string_array_init(ar);
	return ar;
}

void string_array_add(StringArray* _this, String* content)
{
	if (_this)
	{
		if (_this->Count >= _this->MaxCount)
		{
			_this->MaxCount *= 2;
			size_t nsz = sizeof(String*) * _this->MaxCount;
			String** items = (String**)realloc(_this->Items, nsz);
			if (!items) assert(0);
			_this->Items = items;
		}

		_this->Items[_this->Count] = content;
		_this->Count++;
	}
}

StringArray* string_array_release(StringArray* ar, bool only_data)
{
	if (ar != 0)
	{
		int ix = 0;
		while (ix < ar->Count)
		{
			String* a = ar->Items[ix];
			string_release(a);
			ix++;
		}

		free(ar->Items);

		if (!only_data)
		{
			free(ar);
		}
	}
	return ar;
}


void string_split_param(const char* content, const int length, const char* token, const int token_length, const bool remove_empty, StringArray* array)
{
	int leng = 0;
	int pos = -1;
	int ix = 0;

	if (remove_empty)
	{
		while (ix < length)
		{
			pos = string_index_of(content, length, token, token_length, ix);

			if (pos >= 0)
			{
				leng = pos - ix;
				if (leng > 0)
				{
					String* str = string_sub_new(content, length, ix, leng);
					string_array_add(array, str);
				}
			}
			else break;
			ix = pos + token_length;
		}

		leng = length - ix;
		if (leng > 0)
		{
			String* str = string_sub_new(content, length, ix, leng);
			string_array_add(array, str);
		}
	}
	else
	{
		while (ix < length)
		{
			pos = string_index_of(content, length, token, token_length, ix);

			if (pos >= 0)
			{
				leng = pos - ix;
				String* str = string_sub_new(content, length, ix, leng);
				string_array_add(array, str);
			}
			else break;
			ix = pos + token_length;
		}

		leng = length - ix;
		String* str = string_sub_new(content, length, ix, leng);
		string_array_add(array, str);
	}
	return array;
}

StringArray* string_split(const char* content, const int length, const char* token, const int token_length, const bool remove_empty)
{
	StringArray* array = string_array_new();
	string_split_param(content, length, token, token_length, remove_empty, array);
	return array;
}



StringArray* string_split_first_char(const char* content, const int length, const char* token, const int token_length)
{
	StringArray* array = string_array_new();
	int pos = -1;
	int ix = 0;
	while (ix < length)
	{
		int p = string_index_first(content, length, token, token_length, ix, &pos);
		if (p >= 0)
		{
			String* str = string_sub_new(content, length, ix, (pos - ix));
			string_array_add(array, str);
			ix = pos + 1;
		}
		else break;
	}
	return array;
}


char* string_to_upper_copy_achar(const char* content)
{
	int leng = strlen(content);
	char* ar = (char*)malloc((leng + 1) * sizeof(char));
	int ix   = 0;

	while (ix < leng)
	{
		ar[ix] = toupper((unsigned char)content[ix]);
		ix++;
	}
	ar[ix] = 0;
	return ar;
}
char* string_to_lower_copy_achar(const char* content)
{
	int leng = strlen(content);
	char* ar = (char*)malloc((leng + 1) * sizeof(char));
	int ix = 0;

	while (ix < leng)
	{
		ar[ix] = tolower((unsigned char)content[ix]);
		ix++;
	}
	ar[ix] = 0;
	return ar;
}
void string_to_lower_copy_achar_out(const char* content, char* lower)
{
	int leng = strlen(content);
	int ix = 0;

	while (ix < leng)
	{
		lower[ix] = tolower((unsigned char)content[ix]);
		ix++;
	}
	lower[ix] = 0;
}


String* string_to_upper_copy(String* _this)
{
	String* ar = (String*)malloc(sizeof(String));

	ar->Length    = _this->Length;
	ar->MaxLength = _this->Length;
	ar->Data      = (char*)malloc((ar->MaxLength + 1) * sizeof(char));
	ar->Data[0]   = 0;

	int ix = 0;
	while (ix < ar->Length)
	{
		ar->Data[ix] = toupper((unsigned char)_this->Data[ix]);
		ix++;
	}
	ar[ix].Data[ix] = 0;
	return ar;
}


int string_with_content(String* _this)
{
	if (!_this) return 0;
	if (_this->Length <= 0) return 0;

	const char* EMPT = " \r\n\t";

	int it = 0;
	int ix = 0;
	while (ix < _this->Length)
	{
		if (_this->Data[ix] != EMPT[0] || _this->Data[ix] != EMPT[0] || _this->Data[ix] != EMPT[0] || _this->Data[ix] != EMPT[0])
		{
			return 1;
		}
		ix++;
	}
	return 0;
}


void string_trim_end_by_first_char(String* _this, const char* token)
{
	int e = strlen(token);

	if (_this->Length > 0)
	{
		int ax = 0;
		int ix = _this->Length;
		while (ix > 0)
		{
			ix--;

			ax = 0;
			while (ax < e)
			{
				if (_this->Data[ix] == token[ax])
				{
					ax = -1;
					break;
				}
				ax++;
			}

			if (ax >= 0)
			{
				ix++;
				break;
			}
		}

		if (ix < _this->Length)
		{
			_this->Length = ix;
			_this->Data[_this->Length] = 0;
		}
	}
}

void string_trim(String* _this)
{
	const char* TRIMC = " \r\n";

	if (_this->Length > 0)
	{
		int ix = _this->Length;
		while (ix > 0)
		{
			ix--;
			if (!(_this->Data[ix] == TRIMC[0] || _this->Data[ix] == TRIMC[1] || _this->Data[ix] == TRIMC[2]))
			{
				ix++;
				break;
			}
		}

		int im = 0;
		while (im < ix)
		{
			if (!(_this->Data[im] == TRIMC[0] || _this->Data[im] == TRIMC[1] || _this->Data[im] == TRIMC[2]))
			{
				break;
			}
			im++;
		}

		if (im > 0 || (ix < _this->Length))
		{
			int cnt = ix - im;

			_this->Length = 0;
			while (_this->Length < cnt)
			{
				_this->Data[_this->Length] = _this->Data[im];
				_this->Length++;
				im++;
			}
			_this->Data[_this->Length] = 0;
		}
	}
}

void string_init_sub(String* _this, const char* data, int data_length, int data_start, int data_count)
{
	if (_this)
	{
		int real_count = data_count;
		if ((data_start + data_count) > data_length)
		{
			real_count = data_count - ((data_start + data_count) - data_length);
		}

		string_init_length(_this, real_count);

		int end = data_start + real_count;
		int ix = data_start;
		while (ix < end)
		{
			_this->Data[_this->Length] = data[ix];
			_this->Length++;
			ix++;
		}
		_this->Data[_this->Length] = 0;
	}
}

void string_append_sub(String* _this, const char* data, int data_length, int data_start, int data_count)
{
	if (_this)
	{
		int real_count = data_count;
		if ((data_start + data_count) > data_length)
		{
			real_count = data_count - ((data_start + data_count) - data_length);
		}

		if ((_this->Length + real_count + 1) >= _this->MaxLength)
		{
			_this->MaxLength = ((_this->Length + real_count + 1) + _this->MaxLength) * 2;
			char* content = (char*)realloc(_this->Data, _this->MaxLength * sizeof(char));

			if (!content) assert(0);

			_this->Data = content;
		}

		int end = data_start + real_count;
		int ix = data_start;
		while (ix < end)
		{
			_this->Data[_this->Length] = data[ix];
			_this->Length++;
			ix++;
		}
		_this->Data[_this->Length] = 0;
	}
}

int string_equals_char_range(const char* s1, const char* s2, const int s1_start, const int s1_count)
{
	int leng1 = strlen(s1);
	int leng2 = strlen(s2);

	int end = s1_count >= 0 ? (s1_start + s1_count) : leng1;
	if (end > leng1)
	{
		end = leng1;
	}

	int ixs = s1_start;
	int ix = 0;
	while (ix < leng2 && ixs < end)
	{
		if (s1[ixs] != s2[ix])
		{
			return 0;
		}
		ix++;
		ixs++;
	}
	return 1;
}

int string_equals_range(String* s1, const int s1_start, const int s1_count, const char* s2)
{
	int leng2 = strlen(s2);

	int end = s1_count >= 0 ? (s1_start + s1_count) : s1->Length;
	if (end > s1->Length)
	{
		end = s1->Length;
	}

	int ixs = s1_start;
	int ix = 0;
	while (ix < leng2 && ixs < end)
	{
		if (s1->Data[ixs] != s2[ix])
		{
			return 0;
		}
		ix++;
		ixs++;
	}
	return 1;
}

int string_equals_range_s2leng(String* s1, const int s1_start, const int s1_count, const char* s2, const int s2_length)
{
	int end = s1_count >= 0 ? (s1_start + s1_count) : s1->Length;
	if (end > s1->Length)
	{
		end = s1->Length;
	}

	int ixs = s1_start;
	int ix = 0;
	while (ix < s2_length && ixs < end)
	{
		if (s1->Data[ixs] != s2[ix])
		{
			return 0;
		}
		ix++;
		ixs++;
	}
	return 1;
}

int string_equals_char(const char* s1, const char* s2)
{
	int leng1 = strlen(s1);
	int leng2 = strlen(s2);

	if (leng1 == leng2)
	{
		int ix = 0;
		while (ix < leng1)
		{
			if (s1[ix] != s2[ix])
			{
				return 0;
			}
			ix++;
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

int string_equals_s(String* s1, String* s2)
{
	if (s1->Length == s2->Length)
	{
		int ix = 0;
		while (ix < s1->Length)
		{
			if (s1->Data[ix] != s2->Data[ix])
			{
				return 0;
			}
			ix++;
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

int string_equals(String* s1, const char* s2)
{
	int leng = strlen(s2);

	if (s1->Length == leng)
	{
		int ix = 0;
		while (ix < leng)
		{
			if (s1->Data[ix] != s2[ix])
			{
				return 0;
			}
			ix++;
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

void string_append_char(String* _this, const char data)
{
	if (_this)
	{
		if ((_this->Length + 2) >= _this->MaxLength)
		{
			_this->MaxLength = ((_this->Length + 2) + _this->MaxLength) * 2;
			char* content = (char*)realloc(_this->Data, _this->MaxLength * sizeof(char));
			if (!content) assert(0);
			_this->Data = content;
		}

		_this->Data[_this->Length] = data;
		_this->Length++;
		_this->Data[_this->Length] = 0;
	}
}

void string_append(String* _this, const char* data)
{
	if (_this)
	{
		int leng = strlen(data);

		if ((_this->Length + leng + 1) >= _this->MaxLength)
		{
			_this->MaxLength = ((_this->Length + leng + 1) + _this->MaxLength) * 2;
			char* content = (char*)realloc(_this->Data, _this->MaxLength * sizeof(char));
			if (!content) assert(0);
			_this->Data = content;
		}

		int ix = 0;
		while (ix < leng)
		{
			_this->Data[_this->Length] = data[ix];
			_this->Length++;
			ix++;
		}
		_this->Data[_this->Length] = 0;
	}
}


String* string_write_format(const char* format, ...)
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

	String* result = string_new();
	string_append(result, fstr);
	free(fstr);
	return result;
}

void string_append_format(String* _this, const char* format, ...)
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

	string_append(_this, fstr);
	free(fstr);
}


void string_append_s(String* _this, String* data)
{
	if (data->Length > 0)
	{
		string_append(_this, data->Data);
	}
}

void string_sub(const char* content, const int content_length, const int start, const int count, const int initialize, String* target)
{
	if (count <= 0)
	{
		if (initialize)
		{
			string_init_length(target, 0);
		}
		else
		{
			target->Length = 0;
			target->Data[0] = 0;
		}
		return;
	}

	if (content_length > 0)
	{
		int real_count = count;
		if ((start + count) > content_length)
		{
			real_count = count - ((start + count) - content_length);
		}

		if (initialize)
		{
			string_init_length(target, real_count);
		}
		else if ((target->Length + real_count + 1) >= target->MaxLength)
		{
			target->MaxLength = ((target->Length + real_count + 1) + target->MaxLength) * 2;
			char* cont = (char*)realloc(target->Data, target->MaxLength * sizeof(char));
			if (!cont) assert(0);
			target->Data = cont;
		}

		int k = 0;
		int i = start;
		while ((i < content_length) && (k < real_count))
		{
			target->Data[k] = content[i];
			i++;
			k++;
		}
		target->Data[k] = 0;

		target->Length = real_count;
		target->MaxLength = real_count;

		k = 0;
		i = start;
		while ((i < content_length) && (k < real_count))
		{
			target->Data[k] = content[i];
			i++;
			k++;
		}
		target->Length = k;
		target->Data[k] = '\0';
		return;
	}
	else
	{
		if (initialize)
		{
			string_init_length(target, 0);
		}
		else
		{
			target->Length = 0;
			target->Data[0] = 0;
		}
	}
}


String* string_sub_new(const char* content, const int content_length, const int start, const int count)
{
	String* nw = string_new();
	string_sub(content, content_length, start, count, 0, nw);
	return nw;
}


int string_token_count(const char* data, const int data_length, const char token, const int start, const int count, int* position)
{
	int real_count = count;
	if ((start + count) > data_length)
	{
		real_count = count - ((start + count) - data_length);
	}

	int cnt = 0;
	int i = start;
	while (i < data_length)
	{
		if (data[i] == token)
		{
			cnt++;
			while (i < data_length)
			{
				if (data[i] == token)
				{
					cnt++;
				}
				else
				{
					*position = i;
					return cnt;
				}
				i++;
			}
		}
		else
		{
			*position = i;
			return cnt;
		}
		i++;
	}
	*position = i;
	return cnt;
}


int string_index_of_char(const char* data, const int data_length, const char token, const int start, const int count)
{
	int real_count = count;
	if ((start + count) > data_length)
	{
		real_count = count - ((start + count) - data_length);
	}

	int end = start + real_count;
	int i = start;
	while (i < end)
	{
		if (data[i] == token)
		{
			return i;
		}
		i++;
	}
	return -1;
}

int string_index_end_char(const char* data, const char token)
{
	int i = strlen(data);

	while (i > 0)
	{
		i--;

		if (data[i] == token)
		{
			return i;
		}
	}
	return -1;
}

int string_index_of(const char* data, const int data_length, const char* token, const int token_length, const int start)
{
	int i_token = 0;
	int i = start;
	while (i < data_length)
	{
		if (data[i] == token[i_token])
		{
			i_token++;
			if (i_token >= token_length)
			{
				return i - token_length + 1;
			}
		}
		else
		{
			i_token = 0;
		}
		i++;
	}
	return -1;
}

int string_index_first(const char* data, const int data_length, const char* token, const int token_length, const int start, int* position)
{
	int i_token = 0;
	int i = start;
	while (i < data_length)
	{
		int j = 0;
		while (j < token_length)
		{
			if (data[i] == token[j])
			{
				(*position) = i;
				return j;
			}
			j++;
		}
		i++;
	}
	return -1;
}

int string_index_first_string(const char* data, const int data_length, const int data_start, const char** tokens, const int* tokens_length, const int token_count, int* position)
{
	int i_token = 0;
	int i = data_start;

	while (i < data_length)
	{
		int j = 0;
		while (j < token_count)
		{
			int p = i;
			int k = 0;
			while (k < tokens_length[j])
			{
				if (data[p] != tokens[j][k])
				{
					break;
				}
				p++;
				k++;
			}
			if (k == tokens_length[j])
			{
				(*position) = i;
				return j;
			}
			j++;
		}
		i++;
	}
	return -1;
}

void string_resize_forward(String* content, int position)
{
	if (content->MaxLength > 0)
	{
		int leng = position < content->Length ? content->Length - position : 0;

		if (leng > 0)
		{
			int iz = position;
			int ix = 0;
			while (ix < content->Length)
			{
				content->Data[ix] = content->Data[iz];
				ix++;
				iz++;
			}
			content->Data[ix] = 0;
		}
		else
		{
			content->Length = 0;
			content->Data[0] = 0;
		}
	}
}


int string_walk_while_match(byte* data, int length, int position, char token)
{
	int ix = position;
	while (ix < length)
	{
		if (data[ix] != token)
		{
			break;
		}
		ix++;
	}
	return ix;
}








static int hex_to_int(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return -1;
}


char* string_http_url_decode(const char* src)
{
	char* decoded = malloc(strlen(src) + 1);

	if (!decoded) return 0;

	char* dst = decoded;

	while (*src)
	{
		if (*src == '%')
		{
			// Verifica se há pelo menos dois caracteres após '%'
			if (*(src + 1) && *(src + 2))
			{
				int high = hex_to_int(*(src + 1));
				int low = hex_to_int(*(src + 2));

				if (high >= 0 && low >= 0)
				{
					*dst++ = (char)(high * 16 + low);
					src += 3;
					continue;
				}
			}
		}
		else if (*src == '+')
		{
			// O caractere '+' representa um espaço na URL
			*dst++ = ' ';
			src++;
			continue;
		}

		*dst++ = *src++; // Copia o caractere normal
	}
	*dst = '\0';
	return decoded;
}

char* string_http_url_decode_s(const char* src, size_t count, int* out_length)
{
	char* decoded = malloc(count + 1);

	if (!decoded) return NULL;

	size_t i = 0;
	size_t j = 0;

	while (i < count && src[i] != '\0')
	{
		if (src[i] == '%')
		{
			if (i + 2 < count && src[i + 1] != '\0' && src[i + 2] != '\0')
			{
				int high = hex_to_int(src[i + 1]);
				int low = hex_to_int(src[i + 2]);

				if (high >= 0 && low >= 0)
				{
					decoded[j++] = (char)(high * 16 + low);
					i += 3;
					continue;
				}
			}
			// Se não for uma sequência válida, copia o '%' literalmente
			decoded[j++] = src[i++];
		}
		else if (src[i] == '+')
		{
			// O sinal '+' representa um espaço
			decoded[j++] = ' ';
			i++;
		}
		else
		{
			decoded[j++] = src[i++];
		}
	}
	decoded[j] = '\0';
	return decoded;
}


char* string_http_url_encode(const char* src)
{
	size_t len = strlen(src);
	char* encoded = malloc(3 * len + 1);
	if (!encoded) return NULL;

	char* dst = encoded;
	while (*src)
	{
		if (isalnum((unsigned char)*src) || *src == '-' || *src == '_' || *src == '.' || *src == '~')
		{
			*dst++ = *src;
		}
		// O espaço é convertido para '+'
		else if (*src == ' ')
		{
			*dst++ = '+';
		}
		else
		{
			sprintf_s(dst, len, "%%%02X", (unsigned char)*src);
			dst += 3;
		}
		src++;
	}
	*dst = '\0';
	return encoded;
}

void string_utf8_to_bytes(const char* utf8_str, byte** byte_array, size_t* length)
{
	*length = strlen(utf8_str);
	*byte_array = (byte*)malloc(*length);

	if (*byte_array == NULL)
	{
		perror("Failed to allocate memory in 'utf8_to_byte_array'");
		exit(EXIT_FAILURE);
	}

	memcpy(*byte_array, utf8_str, *length);
}

char* string_bytes_to_utf8(byte* byte_array, size_t length)
{
	char* str = (char*)malloc(length + 1);
	if (str == NULL)
	{
		perror("Falha ao alocar memória");
		return NULL;
	}

	memcpy(str, byte_array, length);
	str[length] = '\0';
	return str;
}

char* string_base64_encode(const byte* data, size_t input_length)
{
	// Calcula o tamanho da saída Base64 (multiplo de 4)
	size_t output_length = 4 * ((input_length + 2) / 3);

	// Aloca memória para a string codificada (+1 para o caractere nulo)
	char* encoded_data = malloc(output_length + 1);
	if (encoded_data == NULL) return NULL;

	for (size_t i = 0, j = 0; i < input_length;) 
	{
		// Lê os próximos três bytes (ou 0 se não houver bytes suficientes)
		uint32_t octet_a = i < input_length ? data[i++] : 0;
		uint32_t octet_b = i < input_length ? data[i++] : 0;
		uint32_t octet_c = i < input_length ? data[i++] : 0;

		// Concatena os três bytes em um inteiro de 24 bits
		uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

		// Separa os 24 bits em quatro grupos de 6 bits e mapeia para os caracteres da tabela
		encoded_data[j++] = BASE64_TABLE[(triple >> 18) & 0x3F];
		encoded_data[j++] = BASE64_TABLE[(triple >> 12) & 0x3F];
		encoded_data[j++] = BASE64_TABLE[(triple >> 6) & 0x3F];
		encoded_data[j++] = BASE64_TABLE[triple & 0x3F];
	}

	// Adiciona os caracteres '=' de acordo com o tamanho dos dados de entrada
	for (int i = 0; i < BASE64_MOD_TABLE[input_length % 3]; i++)
	{
		encoded_data[output_length - 1 - i] = '=';
	}

	// Finaliza a string
	encoded_data[output_length] = '\0';
	return encoded_data;
}

byte* string_base64_decode(const char* data, size_t input_length, size_t* output_length) 
{
	// A string Base64 deve ter tamanho múltiplo de 4
	if (input_length % 4 != 0) return NULL;

	// Calcula o tamanho da saída
	*output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=') (*output_length)--;
	if (data[input_length - 2] == '=') (*output_length)--;

	byte* decoded_data = malloc(*output_length);
	if (decoded_data == NULL) return NULL;

	// Cria uma tabela de decodificação: inicializa todos os índices com -1
	int decoding_table[256];
	for (int i = 0; i < 256; i++) 
	{
		decoding_table[i] = -1;
	}
	for (int i = 0; i < 64; i++)
	{
		decoding_table[(byte)BASE64_TABLE[i]] = i;
	}

	// Processa a entrada em blocos de 4 caracteres
	for (size_t i = 0, j = 0; i < input_length;) 
	{
		int sextet_a = data[i] == '=' ? 0 : decoding_table[(byte)data[i]]; i++;
		int sextet_b = data[i] == '=' ? 0 : decoding_table[(byte)data[i]]; i++;
		int sextet_c = data[i] == '=' ? 0 : decoding_table[(byte)data[i]]; i++;
		int sextet_d = data[i] == '=' ? 0 : decoding_table[(byte)data[i]]; i++;

		int triple = (sextet_a << 18) | (sextet_b << 12) | (sextet_c << 6) | sextet_d;

		if (j < *output_length) decoded_data[j++] = (triple >> 16) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = triple & 0xFF;
	}

	return decoded_data;
}


