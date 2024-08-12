#include "pch.h"
#include "TcpNetwork.h"
#include "SocketAddress.h"
#include "TCPSocket.h"
#include "SocketUtil.h"
#include "Utils.h"

std::unique_ptr<TCPProtocol> CreateTCPProtocol()
{
	return std::move(std::make_unique<TcpNetwork>());
}

using namespace SocketUtil;

TcpNetwork::TcpNetwork() :
	m_tcpSocket{ nullptr }
{}

TcpNetwork::~TcpNetwork()
{
	Cleanup();
}

bool TcpNetwork::Setup(HostType type, const std::string& addr)
{
	switch (type)
	{
	case HostType::Server: return Listen(addr);
	case HostType::Client: return Connect(addr);
	}
	
	return false;
}

bool TcpNetwork::Listen(const std::string& addr)
{
	ReturnIfFalse(Startup());

	sockaddr recvAddr{};
	ReturnIfFalse(GetAddressInfo(addr, &recvAddr));

	SocketAddress sockAddress(recvAddr);

	std::unique_ptr<TCPSocket> listenSocket = CreateTCPSocket(AF_INET);
	if (listenSocket == nullptr) return false;

	ReturnIfFalse(listenSocket->Bind(sockAddress));
	ReturnIfFalse(listenSocket->Listen(SOMAXCONN));

	SocketAddress clientAddr;
	m_tcpSocket = listenSocket->Accept(clientAddr);
	if (m_tcpSocket == nullptr) return false;

	return true;
}

bool TcpNetwork::Connect(const std::string& addr)
{
	ReturnIfFalse(Startup());

	sockaddr recvAddr{};
	ReturnIfFalse(GetAddressInfo(addr, &recvAddr));

	SocketAddress sockAddress(recvAddr);

	m_tcpSocket = CreateTCPSocket(AF_INET);
	if (m_tcpSocket == nullptr) return false;

	return m_tcpSocket->Connect(sockAddress);
}

bool TcpNetwork::Send(const void* data, size_t len, int32_t* recvBytes)
{
	if (m_tcpSocket == nullptr) return false;

	return m_tcpSocket->Send(data, len, recvBytes);
}

bool TcpNetwork::Receive(void* data, size_t len, int32_t* recvBytes)
{
	if (m_tcpSocket == nullptr) return false;
	
	return m_tcpSocket->Receive(data, len, recvBytes);
}

bool TcpNetwork::Shutdown(int shutdownFlag)
{
	return m_tcpSocket->Shutdown(shutdownFlag);
}