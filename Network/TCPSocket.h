#pragma once

class SocketAddress;

class TCPSocket
{
public:
	TCPSocket(SOCKET s);
	~TCPSocket();

	TCPSocket() = delete;
	TCPSocket(const TCPSocket&) = delete;
	TCPSocket& operator=(const TCPSocket&) = delete;

	bool Bind(SocketAddress& bindAddr);
	bool Listen(int backLog = 32);
	std::unique_ptr<TCPSocket> Accept(SocketAddress& fromAddr);

private:
	SOCKET m_socket;
};