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

bool TcpNetwork::Setup(HostType type, const std::string& serverAddress)
{
	ReturnIfFalse(Startup());

	sockaddr serverAddr{};
	ReturnIfFalse(GetAddressInfo(serverAddress, &serverAddr));

	SocketAddress socketServerAddr(serverAddr);

	switch (type)
	{
	case HostType::Server: return Listen(socketServerAddr);
	case HostType::Client: return Connect(socketServerAddr);
	}
	
	return false;
}

bool TcpNetwork::Listen(const SocketAddress& serverAddr)
{
	std::unique_ptr<TCPSocket> listenSocket = CreateTCPSocket(AF_INET);
	if (listenSocket == nullptr) return false;

	ReturnIfFalse(listenSocket->ReuseAddr());
	ReturnIfFalse(listenSocket->Bind(serverAddr));
	ReturnIfFalse(listenSocket->Listen(SOMAXCONN));

	SocketAddress clientAddr;
	m_tcpSocket = listenSocket->Accept(clientAddr);
	if (m_tcpSocket == nullptr) return false;

	//SocketAddress clientAddr2;
	//m_tcpSocket = listenSocket->Accept(clientAddr2);
	//if (m_tcpSocket == nullptr) return false;

	//m_tcpSocket->SetNonBlockingMode(true);

	return true;
}

bool TcpNetwork::Connect(const SocketAddress& serverAddr)
{
	m_tcpSocket = CreateTCPSocket(AF_INET);
	if (m_tcpSocket == nullptr) return false;

	return m_tcpSocket->Connect(serverAddr);
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