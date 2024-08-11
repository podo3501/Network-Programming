#pragma once
#include <WinSock2.h>

class Server
{
public:
	virtual ~Server() {}

	virtual bool Setup(const std::string& addr) = 0;
	virtual bool PrepareTCPSocket() = 0;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) = 0;
};

std::unique_ptr<Server> CreateServer();

class Client
{
public:
	virtual ~Client() {}

	virtual bool Setup(const std::string& addr) = 0;
	virtual bool Connect() = 0;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes) = 0;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) = 0;
};

std::unique_ptr<Client> CreateClient();