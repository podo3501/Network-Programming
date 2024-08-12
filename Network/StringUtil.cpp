#include "pch.h"
#include "StringUtil.h"
#include <corecrt_wstdio.h>
#include <stdarg.h>

void StringUtils::Log(const std::wstring format, ...)
{
	//not thread safe...
	std::wstring msg;
	msg.resize(4096);

	va_list args;
	va_start(args, format);

	_vsnwprintf_s(msg.data(), 4096, 4096, format.c_str(), args);
	msg.append(L"\n");

	OutputDebugString(msg.c_str());
}

std::string StringUtils::Sprintf(const char* inFormat, ...)
{
	//not thread safe...
	static char temp[4096];

	va_list args;
	va_start(args, inFormat);

	_vsnprintf_s(temp, 4096, 4096, inFormat, args);

	return std::string(temp);
}