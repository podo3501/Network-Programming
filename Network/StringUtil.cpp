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
