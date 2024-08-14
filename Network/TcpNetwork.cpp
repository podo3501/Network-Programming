#include "pch.h"
#include "TcpNetwork.h"
#include "SocketAddress.h"
#include "TCPSocket.h"
#include "SocketUtil.h"
#include "Utils.h"
#include "Types.h"

std::unique_ptr<TCPProtocol> CreateTCPProtocol()
{
	return std::move(std::make_unique<TcpNetwork>());
}

using namespace SocketUtil;

TcpNetwork::TcpNetwork() :
	m_listenSocket{ nullptr },
	m_tcpSocket{ nullptr },
	m_socketList{},
	m_receiveList{},
	m_fdMax{ 0 }
{
	FD_ZERO(&m_reads); 

	Startup();
}

TcpNetwork::~TcpNetwork()
{
	Cleanup();
}

bool TcpNetwork::Connection(HostType type, const std::string& serverAddress)
{
	switch (type)
	{
	case HostType::Server: return CreateBindSocket(serverAddress);
	case HostType::Client: return Connect(serverAddress);
	}

	return false;
}

void TcpNetwork::AddSocket(std::unique_ptr<TCPSocket> tcpSocket)
{
	m_fdMax = max(m_fdMax, static_cast<int>(tcpSocket->Data()));
	FD_SET(tcpSocket->Data(), &m_reads);

	m_socketList.insert(std::make_pair(tcpSocket->Data(), std::move(tcpSocket)));
}

void TcpNetwork::RemoveSocket(SOCKET socket)
{
	FD_CLR(socket, &m_reads);

	m_socketList.erase(socket);
}

bool TcpNetwork::CreateBindSocket(const std::string& serverAddress)
{
	sockaddr serverAddr{};
	ReturnIfFalse(GetAddressInfo(serverAddress, &serverAddr));

	SocketAddress socketServerAddr(serverAddr);

	std::unique_ptr<TCPSocket> listenSocket = CreateTCPSocket(AF_INET, SocketType::Server);
	if (listenSocket == nullptr) return false;

	ReturnIfFalse(listenSocket->ReuseAddr());
	ReturnIfFalse(listenSocket->Bind(socketServerAddr));
	ReturnIfFalse(listenSocket->Listen(SOMAXCONN));
	//ReturnIfFalse(listenSocket->SetNonBlockingMode(true));

	AddSocket(std::move(listenSocket));

	return true;
}

bool TcpNetwork::UpdateSocket(bool* isChange)
{
	fd_set cpy_reads{ m_reads };
	timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 5000;
	auto socketListCount = static_cast<int>(m_socketList.size());
	auto fd_num = select(m_fdMax + 1, &cpy_reads, 0, 0, &timeout);
	if (fd_num == -1)
		return false;

	if (fd_num == 0)
	{
		(*isChange) = false;
		return true;
	}

	for (auto& iterSocket : m_socketList)
	{
		if (fd_num <= 0) break;

		TCPSocket* socket = (iterSocket).second.get();
		if (!FD_ISSET(socket->Data(), &cpy_reads))
			continue;

		if (socket->Type() == SocketType::Server)
		{
			SocketAddress clientAddr;
			std::unique_ptr<TCPSocket> client = socket->Accept(clientAddr);
			if (client == nullptr) return false;

			AddSocket(std::move(client));
			fd_num--;
			continue;
		}

		if (socket->Type() == SocketType::Client)
		{
			m_receiveList.push(socket);
			
			(*isChange) = true;
		}
	}

	return true;
}

//bool TcpNetwork::Accept(const std::string& serverAddress)
//{
//	if (m_listenSocket == nullptr)
//	{
//		sockaddr serverAddr{};
//		ReturnIfFalse(GetAddressInfo(serverAddress, &serverAddr));
//
//		SocketAddress socketServerAddr(serverAddr);
//
//		m_listenSocket = CreateTCPSocket(AF_INET, SocketType::Server);
//		if (m_listenSocket == nullptr) return false;
//
//		ReturnIfFalse(m_listenSocket->ReuseAddr());
//		ReturnIfFalse(m_listenSocket->Bind(socketServerAddr));
//		ReturnIfFalse(m_listenSocket->Listen(SOMAXCONN));
//		ReturnIfFalse(m_listenSocket->SetNonBlockingMode(true));
//	}
//
//	SocketAddress clientAddr;
//	std::unique_ptr<TCPSocket> tcpSocket = m_listenSocket->Accept(clientAddr);
//	if (tcpSocket == nullptr) return false;
//
//	m_tcpSocket = std::move(tcpSocket);
//	//m_tcpSocketList.emplace_back(std::move(tcpSocket));
//
//	m_listenSocket.reset();
//
//	return true;
//}

bool TcpNetwork::Connect(const std::string& serverAddress)
{
	sockaddr serverAddr{};
	ReturnIfFalse(GetAddressInfo(serverAddress, &serverAddr));

	SocketAddress socketServerAddr(serverAddr);

	m_tcpSocket = CreateTCPSocket(AF_INET, SocketType::Client);
	if (m_tcpSocket == nullptr) return false;

	return m_tcpSocket->Connect(socketServerAddr);
}

bool TcpNetwork::Send(const void* data, size_t len, int32_t* recvBytes)
{
	for (auto& iterSocket : m_socketList)
	{
		auto socket = iterSocket.second.get();
		if (socket->Type() == SocketType::Server)
			continue;
		
		ReturnIfFalse(socket->Send(data, len, recvBytes));
	}

	return true;
}

bool TcpNetwork::Receive(void* data, size_t len, int32_t* recvBytes, bool* exist)
{
	auto socket = m_receiveList.front();
	ReturnIfFalse(socket->Receive(data, len, recvBytes));

	m_receiveList.pop();
	(*exist) = !m_receiveList.empty();

	return true;
}

bool TcpNetwork::Shutdown(int shutdownFlag)
{
	if (m_tcpSocket == nullptr) return true;

	return m_tcpSocket->Shutdown(shutdownFlag);
}

/////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<TCPServer> CreateTCPServer()
{
	return std::move(std::make_unique<CTCPServer>());
}

CTCPServer::CTCPServer() :
	m_socketList{},
	m_receiveList{},
	m_fdMax{ 0 }
{
	FD_ZERO(&m_reads);

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

	std::unique_ptr<TCPSocket> listenSocket = CreateTCPSocket(AF_INET, SocketType::Server);
	if (listenSocket == nullptr) return false;

	ReturnIfFalse(listenSocket->ReuseAddr());
	ReturnIfFalse(listenSocket->Bind(socketServerAddr));
	ReturnIfFalse(listenSocket->Listen(SOMAXCONN));
	//ReturnIfFalse(listenSocket->SetNonBlockingMode(true));

	AddSocket(std::move(listenSocket));

	return true;
}

bool CTCPServer::UpdateSocket(bool* isChange)
{
	fd_set cpy_reads{ m_reads };
	timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 5000;
	auto socketListCount = static_cast<int>(m_socketList.size());
	auto fd_num = select(m_fdMax + 1, &cpy_reads, 0, 0, &timeout);
	if (fd_num == -1)
		return false;

	if (fd_num == 0)
	{
		(*isChange) = false;
		return true;
	}

	for (auto& iterSocket : m_socketList)
	{
		if (fd_num <= 0) break;

		TCPSocket* socket = (iterSocket).second.get();
		if (!FD_ISSET(socket->Data(), &cpy_reads))
			continue;

		if (socket->Type() == SocketType::Server)
		{
			SocketAddress clientAddr;
			std::unique_ptr<TCPSocket> client = socket->Accept(clientAddr);
			if (client == nullptr) return false;

			AddSocket(std::move(client));
			fd_num--;
			continue;
		}

		if (socket->Type() == SocketType::Client)
		{
			m_receiveList.push(socket);

			(*isChange) = true;
		}
	}

	return true;
}

bool CTCPServer::Send(const void* data, size_t len, int32_t* recvBytes)
{
	for (auto& iterSocket : m_socketList)
	{
		auto socket = iterSocket.second.get();
		if (socket->Type() == SocketType::Server)
			continue;

		ReturnIfFalse(socket->Send(data, len, recvBytes));
	}

	return true;
}

bool CTCPServer::Receive(void* data, size_t len, int32_t* recvBytes, bool* exist)
{
	auto socket = m_receiveList.front();
	ReturnIfFalse(socket->Receive(data, len, recvBytes));

	m_receiveList.pop();
	(*exist) = !m_receiveList.empty();

	return true;
}

bool CTCPServer::Shutdown(int shutdownFlag)
{
	auto serv = std::ranges::find_if(m_socketList, [](auto& iter) {
		return iter.second->Type() == SocketType::Server;
		});

	return serv->second->Shutdown(shutdownFlag);

	return true;
}

void CTCPServer::AddSocket(std::unique_ptr<TCPSocket> tcpSocket)
{
	m_fdMax = max(m_fdMax, static_cast<int>(tcpSocket->Data()));
	FD_SET(tcpSocket->Data(), &m_reads);

	m_socketList.insert(std::make_pair(tcpSocket->Data(), std::move(tcpSocket)));
}

void CTCPServer::RemoveSocket(SOCKET socket)
{
	FD_CLR(socket, &m_reads);

	m_socketList.erase(socket);
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

	return m_tcpSocket->Connect(socketServerAddr);
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