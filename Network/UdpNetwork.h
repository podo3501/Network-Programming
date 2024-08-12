#pragma once
#include "../Include/NetworkInterface.h"

class UDPSocket;
class SocketAddress;

class UdpNetwork final : public UDPProtocol
{
public:
	UdpNetwork();
	~UdpNetwork();

	UdpNetwork(const UdpNetwork&) = delete;
	UdpNetwork& operator=(const UdpNetwork&) = delete;

	virtual bool SendTo(const void* send, size_t length, const SocketAddress& address, int32_t* sentBytes) override;
	virtual bool ReceiveFrom(void* receive, int maxLength, SocketAddress& outFromAddress, int32_t* recvBytes) override;

	bool CreateSocket();
	bool Bind(const std::string& fullAddr);

private:
	std::unique_ptr<UDPSocket> m_udpSocket;
};
