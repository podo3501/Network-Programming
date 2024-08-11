#include "pch.h"
#include "TCPSocket.h"
#include "SocketAddress.h"
#include "SocketUtil.h"

using namespace SocketUtil;

TCPSocket::TCPSocket(SOCKET s) :
	m_socket(s)
{
}

TCPSocket::~TCPSocket()
{
	closesocket(m_socket);
}

bool TCPSocket::Bind(const SocketAddress& bindAddr)
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

bool TCPSocket::Connect(const SocketAddress& addr)
{
	int error = connect(m_socket, &addr.GetData(), static_cast<int>(addr.GetSize()));
	if (error < 0)
	{
		ReportError(L"TCPSocket::Connect");
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

bool TCPSocket::Send(const void* data, size_t len, int32_t* recvBytes)
{
	auto receiveBytes = send(m_socket, static_cast<const char*>(data), static_cast<int>(len), 0);
	if (receiveBytes < 0)
	{
		ReportError(L"TCPSocket::Send");
		return false;
	}

	if (recvBytes != nullptr) (*recvBytes) = receiveBytes;

	return true;
}

bool TCPSocket::Receive(void* data, size_t len, int32_t* recvBytes)
{
	auto receiveBytes = recv(m_socket, static_cast<char*>(data), static_cast<int>(len), 0);
	if (receiveBytes < 0)
	{
		ReportError(L"TCPSocket::Receive");
		return false;
	}

	if (recvBytes != nullptr) (*recvBytes) = receiveBytes;

	return true;
}

bool TCPSocket::Shutdown(int shutdownFlag)
{
	auto result = shutdown(m_socket, shutdownFlag);
	if (result == SOCKET_ERROR)
	{
		ReportError(L"TCPSocket::Shutdown");
		return false;
	}

	return true;
}

