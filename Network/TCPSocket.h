#pragma once

class SocketAddress;
struct ReceiveData;
enum class SocketType;

class TCPSocket
{
public:
	TCPSocket(SOCKET s, SocketType type);
	~TCPSocket();

	TCPSocket() = delete;
	TCPSocket(const TCPSocket&) = delete;
	TCPSocket& operator=(const TCPSocket&) = delete;

	bool ReuseAddr();
	bool Bind(const SocketAddress& bindAddr);
	bool Listen(int backLog = 32);
	bool Connect(const SocketAddress& addr);
	std::unique_ptr<TCPSocket> Accept(SocketAddress& fromAddr);
	bool Send(const void* data, size_t len, int32_t* recvBytes);
	bool Receive(void* data, size_t len, int32_t* recvBytes);
	bool Receive(ReceiveData& outData);
	bool SetNonBlockingMode(bool nonBlocking);
	bool Shutdown(int shutdownFlag);
	SOCKET Data() { return m_socket; }
	SocketType Type() { return m_type; }

private:
	SOCKET m_socket;
	SocketType m_type;
};