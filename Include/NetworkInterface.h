#pragma once
#include <WinSock2.h>

class TcpCommon
{
public:
	virtual ~TcpCommon() {}

	virtual bool Setup(const std::string& addr) = 0;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) = 0;
};

class TcpServer : public TcpCommon
{
public:
	virtual ~TcpServer() {}

	virtual bool Listen() = 0;
};

std::unique_ptr<TcpServer> CreateServer();

class TcpClient : public TcpCommon
{
public:
	virtual ~TcpClient() {}

	virtual bool Connect() = 0;
};

std::unique_ptr<TcpClient> CreateClient();