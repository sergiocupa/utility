#include "../include/stringlib.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>


#define INITIAL_STRING_LENGTH       200
#define INITIAL_STRING_ARRAY_LENGTH 100




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
	if (ar)
	{
		free(ar->Data);
	}
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


StringArray* string_split(const char* content, const int length, const char* token, const int token_length)
{
	StringArray* array = string_array_new();
	int pos = -1;
	int ix = 0;
	while (ix < length)
	{
		pos = string_index_of(content, length, token, token_length, ix);

		if (pos >= 0)
		{
			String* str = string_sub_new(content, length, ix, (pos - ix));
			string_array_add(array, str);
		}
		else break;
		ix = pos + token_length;
	}
	return array;
}

void string_split_param(const char* content, const int length, const char* token, const int token_length, StringArray* array)
{
	int pos = -1;
	int ix = 0;
	while (ix < length)
	{
		pos = string_index_of(content, length, token, token_length, ix);

		if (pos >= 0)
		{
			String* str = string_sub_new(content, length, ix, (pos - ix));
			string_array_add(array, str);
		}
		else break;
		ix = pos + token_length;
	}
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
	while (ix < leng1 && ixs < end)
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

String* string_append_format(const char* format, ...)
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

void string_append_s(String* _this, String* data)
{
	if (data->Length > 0)
	{
		string_append(_this, data->Data);
	}
}

inline void string_sub(const char* content, const int content_length, const int start, const int count, const int initialize, String* target)
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


inline String* string_sub_new(const char* content, const int content_length, const int start, const int count)
{
	String* nw = string_new();
	string_sub(content, content_length, start, count, 0, nw);
	return nw;
}


inline int string_token_count(const char* data, const int data_length, const char token, const int start, const int count, int* position)
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


inline int string_index_of_char(const char* data, const int data_length, const char token, const int start, const int count)
{
	int real_count = count;
	if ((start + count) > data_length)
	{
		real_count = count - ((start + count) - data_length);
	}

	int i = start;
	while (i < data_length)
	{
		if (data[i] == token)
		{
			return i;
		}
		i++;
	}
	return -1;
}

inline int string_index_end_char(const char* data, const char token)
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

inline int string_index_of(const char* data, const int data_length, const char* token, const int token_length, const int start)
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

inline int string_index_first(const char* data, const int data_length, const char* token, const int token_length, const int start, int* position)
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

inline int string_index_first_string(const char* data, const int data_length, const int data_start, const char** tokens, const int* tokens_length, const int token_count, int* position)
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

