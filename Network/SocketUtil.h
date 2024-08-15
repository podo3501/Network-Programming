#pragma once
#include "Define.h"

class TCPSocket;
class UDPSocket;
enum class SocketType;

namespace SocketUtil
{
	void ReportError(const std::wstring errorDesc);

	bool Startup();
	bool Cleanup();
	void GetHostAndService(const std::string& addr, std::string* host, std::string* service);
	bool GetAddressInfo(const std::string& fullAddr, sockaddr* outAddr);
	TCPSocketPtr CreateTCPSocket(int addrFamily, SocketType type);
	std::unique_ptr<UDPSocket> CreateUDPSocket(int addrFamily);

	int Select(const TCPSocketPtrList* inReadSet, TCPSocketPtrList* outReadSet,
		const TCPSocketPtrList* inWriteSet, TCPSocketPtrList* outWriteSet,
		const TCPSocketPtrList* inExceptSet, TCPSocketPtrList* outExceptSet);
}