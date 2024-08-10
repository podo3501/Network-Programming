#include "pch.h"
#include "Network.h"
#include "SocketAddress.h"
#include "TCPSocket.h"
#include "SocketUtil.h"
#include "Utils.h"

std::unique_ptr<Server> CreateServer()
{
	return std::move(std::make_unique<Network>());
}

using namespace SocketUtil;

Network::Network() :
	m_sockAddress{ nullptr },
	m_listenSocket{ nullptr },
	m_clientSocket{ nullptr }
{}

Network::~Network()
{
	WSACleanup();
}

bool Network::Setup(const std::string& addr)
{
	ReturnIfFalse(Startup());
	ReturnIfFalse(CreateIPv4(addr));

	return true;
}

bool Network::Startup()
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

bool Network::PrepareTCPSocket()
{
	m_listenSocket = CreateTCPSocket(AF_INET);
	if (m_listenSocket == nullptr) return false;
	ReturnIfFalse(m_listenSocket->Bind(*m_sockAddress.get()));
	ReturnIfFalse(m_listenSocket->Listen(SOMAXCONN));

	SocketAddress addr;
	m_clientSocket = m_listenSocket->Accept(addr);
	if (m_clientSocket == nullptr) return false;

	m_listenSocket.reset();

	//recv �� ����

	return true;
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

bool Network::CreateIPv4(const std::string& inString)
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