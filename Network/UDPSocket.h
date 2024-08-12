#pragma once

class SocketAddress;

class UDPSocket
{
public:
	UDPSocket(SOCKET s);
	~UDPSocket();

	UDPSocket() = delete;
	UDPSocket(const UDPSocket&) = delete;
	UDPSocket& operator=(const UDPSocket&) = delete;

	bool Bind(const SocketAddress& addr);
	bool SendTo(const void* send, size_t length, const SocketAddress& address, int32_t* sentBytes);
	bool ReceiveFrom(void* receive, size_t maxLength, SocketAddress& outFromAddress, int32_t* recvBytes);

private:
	SOCKET m_socket;
};