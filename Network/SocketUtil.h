#pragma once

class TCPSocket;
class UDPSocket;

namespace SocketUtil
{
	void ReportError(const std::wstring errorDesc);

	bool Startup();
	bool Cleanup();
	void GetHostAndService(const std::string& addr, std::string* host, std::string* service);
	bool GetAddressInfo(const std::string& fullAddr, sockaddr* outAddr);
	std::unique_ptr<TCPSocket> CreateTCPSocket(int addrFamily);
	std::unique_ptr<UDPSocket> CreateUDPSocket(int addrFamily);
}