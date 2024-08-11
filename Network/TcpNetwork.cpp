#include "pch.h"
#include "TcpNetwork.h"
#include "SocketAddress.h"
#include "TCPSocket.h"
#include "SocketUtil.h"
#include "Utils.h"

std::unique_ptr<TcpServer> CreateServer()
{
	return std::move(std::make_unique<TcpNetwork>());
}

std::unique_ptr<TcpClient> CreateClient()
{
	return std::move(std::make_unique<TcpNetwork>());
}

using namespace SocketUtil;

TcpNetwork::TcpNetwork() :
	m_sockAddress{ nullptr },
	m_tcpSocket{ nullptr }
{}

TcpNetwork::~TcpNetwork()
{
	WSACleanup();
}

bool TcpNetwork::Setup(const std::string& addr)
{
	ReturnIfFalse(Startup());
	ReturnIfFalse(CreateIPv4(addr));

	return true;
}

bool TcpNetwork::Startup()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		ReportError(L"Startup");
		return false;
	}

	return true;
}

bool TcpNetwork::Listen()
{
	std::unique_ptr<TCPSocket> listenSocket = CreateTCPSocket(AF_INET);
	if (listenSocket == nullptr) return false;

	ReturnIfFalse(listenSocket->Bind(*m_sockAddress.get()));
	ReturnIfFalse(listenSocket->Listen(SOMAXCONN));

	SocketAddress addr;
	m_tcpSocket = listenSocket->Accept(addr);
	if (m_tcpSocket == nullptr) return false;

	return true;
}

bool TcpNetwork::Connect()
{
	m_tcpSocket = CreateTCPSocket(AF_INET);
	if (m_tcpSocket == nullptr) return false;

	return m_tcpSocket->Connect(*m_sockAddress.get());
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

void GetHostAndService(const std::string& addr, std::string* host, std::string* service)
{
	std::string copyAddr = addr;
	auto pos = copyAddr.find_last_of(':');
	if (pos != std::string::npos)
	{
		(*host) = copyAddr.substr(0, pos);
		(*service) = copyAddr.substr(pos + 1);
	}
	else
	{
		(*host) = copyAddr;
		//use default port...
		(*service) = "0";
	}
}

bool TcpNetwork::CreateIPv4(const std::string& inString)
{
	std::string host, service;
	GetHostAndService(inString, &host, &service);

	addrinfo hint{};
	hint.ai_family = AF_INET;

	addrinfo* result;
	int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
	if (error != 0 && result != nullptr)
	{
		ReportError(L"CreateIPv4");
		return false;
	}

	while (!result->ai_addr && result->ai_next) 
	{ 
		result = result->ai_next; 
	}

	if (!result->ai_addr) 
		return false;

	m_sockAddress = std::make_unique<SocketAddress>(*result->ai_addr);

	freeaddrinfo(result);

	return true;
}