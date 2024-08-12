#pragma once
#include <WinSock2.h>
#include <memory>
#include <string>

class SocketAddress;

enum class HostType
{
	Server,
	Client,
};

// TCP ///////////////////////////////////////////

class TCPProtocol
{
public:
	virtual ~TCPProtocol() {}

	virtual bool Setup(HostType type, const std::string& addr) = 0;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) = 0;
};

std::unique_ptr<TCPProtocol> CreateTCPProtocol();

// UDP ///////////////////////////////////////////

class UDPProtocol
{
public:
	virtual ~UDPProtocol() {}

	virtual bool SendTo(const void* send, size_t length, const SocketAddress& address, int32_t* sentBytes) = 0;
	virtual bool ReceiveFrom(void* receive, int maxLength, SocketAddress& outFromAddress, int32_t* recvBytes) = 0;
};

std::unique_ptr<UDPProtocol> CreateUDPServer(const std::string& fullAddr);
std::unique_ptr<UDPProtocol> CreateUDPClient();