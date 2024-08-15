#pragma once
#include "../Include/NetworkInterface.h"
#include "Define.h"

class SocketAddress;
class TCPSocket;

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
	void AddSocket(std::shared_ptr<TCPSocket> tcpSocket);
	void RemoveSocket(SOCKET socket);

	TCPSocketPtrList m_newSocketList;
	TCPSocketPtrList m_readableSocketList;
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
	TCPSocketPtr m_tcpSocket;

};
