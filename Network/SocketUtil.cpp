#include "pch.h"
#include "SocketUtil.h"
#include "StringUtil.h"
#include "TCPSocket.h"

void SocketUtil::ReportError(const std::wstring errorDesc)
{
	LPVOID lpMsgBuf;
	DWORD errorNum = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	StringUtils::Log(L"Error %s: %d - %s", errorDesc.c_str(), errorNum, lpMsgBuf);
}

std::unique_ptr<TCPSocket> SocketUtil::CreateTCPSocket(int addrFamily)
{
	SOCKET s = socket(addrFamily, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		ReportError(L"CreateTCPSocket");
		return nullptr;
	}

	return std::move(std::make_unique<TCPSocket>(s));
}