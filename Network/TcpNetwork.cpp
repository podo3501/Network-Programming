#include "pch.h"
#include "TcpNetwork.h"
#include "SocketAddress.h"
#include "TCPSocket.h"
#include "SocketUtil.h"
#include "Utils.h"
#include "Types.h"

using namespace SocketUtil;

std::unique_ptr<TCPServer> CreateTCPServer()
{
	return std::move(std::make_unique<CTCPServer>());
}

CTCPServer::CTCPServer() :
	m_newSocketList{},
	m_readableSocketList{}
{
	Startup();
}

CTCPServer::~CTCPServer()
{
	Cleanup();
}

bool CTCPServer::Bind(const std::string& serverAddress)
{
	sockaddr serverAddr{};
	ReturnIfFalse(GetAddressInfo(serverAddress, &serverAddr));

	SocketAddress socketServerAddr(serverAddr);
	
	std::shared_ptr<TCPSocket> listenSocket = CreateTCPSocket(AF_INET, SocketType::Server);
	if (listenSocket == nullptr) return false;

	ReturnIfFalse(listenSocket->ReuseAddr());
	ReturnIfFalse(listenSocket->Bind(socketServerAddr));
	ReturnIfFalse(listenSocket->Listen(SOMAXCONN));
	//ReturnIfFalse(listenSocket->SetNonBlockingMode(true));

	AddSocket(listenSocket);

	return true;
}

bool CTCPServer::UpdateSocket(bool* isChange)
{
	TCPSocketPtrList readableList;
	(*isChange) = false;
	if (!Select(&m_newSocketList, &readableList, nullptr, nullptr, nullptr, nullptr))
		return true;

	for (auto& socket : readableList)
	{
		if (socket->Type() == SocketType::Server)
		{
			SocketAddress clientAddr;
			TCPSocketPtr client = socket->Accept(clientAddr);
			if (client == nullptr) return false;

			AddSocket(client);
			continue;
		}

		if (socket->Type() == SocketType::Client)
		{
			m_readableSocketList.emplace_back(socket);

			(*isChange) = true;
		}
	}

	return true;
}

bool CTCPServer::Send(const void* data, size_t len, int32_t* recvBytes)
{
	for (auto& socket : m_newSocketList)
	{
		if (socket->Type() == SocketType::Server)
			continue;

		ReturnIfFalse(socket->Send(data, len, recvBytes));
	}

	return true;
}

bool CTCPServer::Receive(void* data, size_t len, int32_t* recvBytes, bool* exist)
{
	auto socket = m_readableSocketList.back();
	ReturnIfFalse(socket->Receive(data, len, recvBytes));

	if ((*recvBytes) == 0)
		RemoveSocket(socket);

	m_readableSocketList.pop_back();
	(*exist) = !m_readableSocketList.empty();

	return true;
}

bool CTCPServer::Shutdown(int shutdownFlag)
{
	auto serv = std::ranges::find_if(m_newSocketList, [](auto& socket) {
		return socket->Type() == SocketType::Server;
		});

	return (*serv)->Shutdown(shutdownFlag);
}

void CTCPServer::AddSocket(TCPSocketPtr socket)
{
	m_newSocketList.emplace_back(socket);
}

void CTCPServer::RemoveSocket(TCPSocketPtr socket)
{
	std::erase(m_newSocketList, socket);
}

////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<TCPClient> CreateTCPClient ()
{
	return std::move(std::make_unique<CTCPClient>());
}

CTCPClient::CTCPClient() :
	m_tcpSocket{ nullptr }
{
	Startup();
}

CTCPClient::~CTCPClient()
{
	Cleanup();
}

bool CTCPClient::Connect(const std::string& serverAddress)
{
	sockaddr serverAddr{};
	ReturnIfFalse(GetAddressInfo(serverAddress, &serverAddr));

	SocketAddress socketServerAddr(serverAddr);

	m_tcpSocket = CreateTCPSocket(AF_INET, SocketType::Client);
	if (m_tcpSocket == nullptr) return false;

	ReturnIfFalse(m_tcpSocket->Connect(socketServerAddr));
	ReturnIfFalse(m_tcpSocket->SetNonBlockingMode(true));

	return true;
}

bool CTCPClient::Send(const void* data, size_t len, int32_t* recvBytes)
{
	if (m_tcpSocket == nullptr) return false;

	return m_tcpSocket->Send(data, len, recvBytes);
}

bool CTCPClient::Receive(void* data, size_t len, int32_t* recvBytes)
{
	if (m_tcpSocket == nullptr) return false;

	return m_tcpSocket->Receive(data, len, recvBytes);
}

bool CTCPClient::Shutdown(int shutdownFlag)
{
	if (m_tcpSocket == nullptr) return false;

	return m_tcpSocket->Shutdown(shutdownFlag);
}