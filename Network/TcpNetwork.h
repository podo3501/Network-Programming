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

	virtual bool Connection(HostType type, const std::string& serverAddress) override;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) override;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes, bool* exist) override;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) override;
	virtual bool UpdateSocket(bool* isChange) override;
	
private:
	//bool Accept(const std::string& serverAddress);
	bool Connect(const std::string& serverAddress);
	bool CreateBindSocket(const std::string& serverAddress);
	void AddSocket(std::unique_ptr<TCPSocket> tcpSocket);
	void RemoveSocket(SOCKET socket);

	std::unique_ptr<TCPSocket> m_listenSocket;
	std::unique_ptr<TCPSocket> m_tcpSocket;

	std::map<SOCKET, std::unique_ptr<TCPSocket>> m_socketList;
	fd_set m_reads;
	int m_fdMax;
	std::queue<TCPSocket*> m_receiveList;
};

class CTCPServer : public TCPServer
{
public:
	CTCPServer();
	~CTCPServer();

	CTCPServer(const CTCPServer&) = delete;
	CTCPServer& operator=(const CTCPServer&) = delete;

	virtual bool Bind(const std::string& serverAddress) override;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) override;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes, bool* exist) override;
	virtual bool UpdateSocket(bool* isChange) override;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) override;

private:
	void AddSocket(std::unique_ptr<TCPSocket> tcpSocket);
	void RemoveSocket(SOCKET socket);

	std::map<SOCKET, std::unique_ptr<TCPSocket>> m_socketList;
	fd_set m_reads;
	int m_fdMax;
	std::queue<TCPSocket*> m_receiveList;
};

class CTCPClient : public TCPClient
{
public:
	CTCPClient();
	~CTCPClient();

	CTCPClient(const CTCPClient&) = delete;
	CTCPClient& operator=(const CTCPClient&) = delete;

	virtual bool Connect(const std::string& serverAddress) override;
	virtual bool Send(const void* data, size_t len, int32_t* recvBytes) override;
	virtual bool Receive(void* data, size_t len, int32_t* recvBytes) override;
	virtual bool Shutdown(int shutdownFlag = SD_SEND) override;

private:
	std::unique_ptr<TCPSocket> m_tcpSocket;

};
