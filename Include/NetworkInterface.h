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

struct ReceiveData
{
	void* data{ nullptr };
	size_t len{ 0 };
	int32_t recvBytes{ 0 };
};

// TCP ///////////////////////////////////////////

class TCPProtocol
{
public:
	virtual ~TCPProtocol() {}

	virtual bool Connection(HostType type, const std::string& serverAddress) = 0;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes, bool* exist) = 0;
	virtual bool UpdateSocket(bool* isChange) = 0;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) = 0;
};

std::unique_ptr<TCPProtocol> CreateTCPProtocol();

class TCPServer
{
public:
	virtual ~TCPServer() {}

	virtual bool Bind(const std::string& serverAddress) = 0;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes, bool* exist) = 0;
	virtual bool UpdateSocket(bool* isChange) = 0;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) = 0;
};

std::unique_ptr<TCPServer> CreateTCPServer();

class TCPClient
{
public:
	virtual ~TCPClient() {}

	virtual bool Connect(const std::string& serverAddress) = 0;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) = 0;
};

std::unique_ptr<TCPClient> CreateTCPClient();

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