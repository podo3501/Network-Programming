#include "pch.h"
#include "SocketUtil.h"
#include "StringUtil.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "Types.h"

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

bool SocketUtil::Startup()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		SocketUtil::ReportError(L"SocketUtil::Startup");
		return false;
	}

	return true;
}

bool SocketUtil::Cleanup()
{
	if (WSACleanup() != NO_ERROR)
	{
		SocketUtil::ReportError(L"SocketUtil::Cleanup");
		return false;
	}

	return true;
}

void SocketUtil::GetHostAndService(const std::string& addr, std::string* host, std::string* service)
{
	std::string copyAddr = addr;
	auto pos = copyAddr.find_last_of(':');
	if (pos != std::string::npos)
	{
		(*host) = copyAddr.substr(0, pos);
		(*service) = copyAddr.substr(pos + 1);
	}
	else
	{
		(*host) = copyAddr;
		//use default port...
		(*service) = "0";
	}
}

bool SocketUtil::GetAddressInfo(const std::string& fullAddr, sockaddr* outAddr)
{
	std::string host, service;
	GetHostAndService(fullAddr, &host, &service);

	addrinfo hint{};
	hint.ai_family = AF_INET;

	addrinfo* result = nullptr;

	int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
	if (error != 0 && result != nullptr)
	{
		SocketUtil::ReportError(L"SocketUtil::GetAddressInfo");
		return false;
	}

	while (!result->ai_addr && result->ai_next)
	{
		result = result->ai_next;
	}

	if (!result->ai_addr)
		return false;

	memcpy(outAddr, result->ai_addr, sizeof(sockaddr));

	freeaddrinfo(result);

	return true;
}

std::unique_ptr<TCPSocket> SocketUtil::CreateTCPSocket(int addrFamily, SocketType type)
{
	SOCKET s = socket(addrFamily, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		ReportError(L"SocketUtil::CreateTCPSocket");
		return nullptr;
	}

	return std::move(std::make_unique<TCPSocket>(s, type));
}

std::unique_ptr<UDPSocket> SocketUtil::CreateUDPSocket(int addrFamily)
{
	SOCKET s = socket(addrFamily, SOCK_DGRAM, IPPROTO_UDP);

	if (s == INVALID_SOCKET)
	{
		ReportError(L"SocketUtil::CreateUDPSocket");
		return nullptr;
	}

	return std::move(std::make_unique<UDPSocket>(s));
}