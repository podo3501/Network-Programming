#include "pch.h"
#include "TCPSocket.h"
#include "SocketAddress.h"
#include "SocketUtil.h"

using namespace SocketUtil;

TCPSocket::TCPSocket(SOCKET s) :
	m_socket(s)
{}

TCPSocket::~TCPSocket()
{
	closesocket(m_socket);
}

bool TCPSocket::Bind(SocketAddress& bindAddr)
{
	auto error = bind(m_socket, &bindAddr.GetData(), static_cast<int>(bindAddr.GetSize()));
	if (error != 0)
	{
		ReportError(L"TCPSocket::Bind");
		return false;
	}

	return true;
}

bool TCPSocket::Listen(int backLog)
{
	int error = listen(m_socket, backLog);
	if (error < 0)
	{
		ReportError(L"TCPSocket::Listen");
		return false;
	}

	return true;
}

std::unique_ptr<TCPSocket> TCPSocket::Accept(SocketAddress& fromAddr)
{
	socklen_t length = static_cast<socklen_t>(fromAddr.GetSize());
	SOCKET s = accept(m_socket, &fromAddr.GetData(), &length);
	if (s == INVALID_SOCKET)
	{
		ReportError(L"TCPSocket::Accept");
		return nullptr;
	}

	return std::move(std::make_unique<TCPSocket>(s));
}