#pragma once
#include "../Include/NetworkInterface.h"

class SocketAddress;
class TCPSocket;

class TcpNetwork final : public TCPProtocol
{
public:
	TcpNetwork();
	~TcpNetwork();

	TcpNetwork(const TcpNetwork&) = delete;
	TcpNetwork& operator=(const TcpNetwork&) = delete;

	virtual bool Setup(HostType type, const std::string& addr) override;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) override;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes) override;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) override;
	
private:
	bool Listen(const std::string& addr);
	bool Connect(const std::string& addr);

	std::unique_ptr<TCPSocket> m_tcpSocket;
};
