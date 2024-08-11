#pragma once
#include "../Include/NetworkInterface.h"

class SocketAddress;
class TCPSocket;

class Network final : public Server, public Client
{
public:
	Network();
	~Network();

	Network(const Network&) = delete;
	Network& operator=(const Network&) = delete;

	virtual bool Setup(const std::string& addr) override;
	virtual bool PrepareTCPSocket() override;
	virtual bool Connect() override;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) override;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes) override;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) override;
	
private:
	bool Startup();
	bool CreateIPv4(const std::string& inString);

	std::unique_ptr<SocketAddress> m_sockAddress;
	std::unique_ptr<TCPSocket> m_tcpSocket;
};
