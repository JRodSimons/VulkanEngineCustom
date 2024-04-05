#include "PCH_Engine.h"
#include "String.h"

#include <stdio.h>
#include <stdarg.h>

char* string_format(const char* format, const char* str1, const char* str2)
{
	char out_string[32000];

	sprintf(out_string, format, str1, str2);

	return out_string;
}

char* string_formatv(const char* format, const char* str1, va_list &var_args)
{
	char out_buffer[10000];
	vsprintf(out_buffer, format, var_args);

	return out_buffer;
}

bool string_compare(const char* str1, const char* str2)
{
	int result = strcmp(str1, str2);

	return result == 0;
}

int string_char_to_int(const char c)
{
	int x;
	x = c;

	return x;
}