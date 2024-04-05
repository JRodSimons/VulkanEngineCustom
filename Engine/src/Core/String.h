#pragma once

#include "EngineGlobals.h"

ENGINE_API char* string_format(const char* format, const char* str1, const char* str2);

ENGINE_API char* string_formatv(const char* format, const char* str1, va_list &var_args);

ENGINE_API bool string_compare(const char* str1, const char* str2);

int string_char_to_int(const char c);