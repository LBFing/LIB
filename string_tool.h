#pragma once
#include "type_define.h"

class StringTool
{
public:
	//static const string WHITE_SPACE = " \t\r\n\f\v";
	static const int DEFAULT_BUFFER_SIZE = 1024;
public:
	static string& format(string& resultOut, const char* fmt, ...);
	static string  format(const char* fmt, ...);
	static string  vformat(string& resultOut, const char* fmt, va_list argptr, int32 default_size = DEFAULT_BUFFER_SIZE);
};
