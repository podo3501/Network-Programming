#include "pch.h"
#include "UDPSocket.h"
#include "SocketAddress.h"
#include "SocketUtil.h"
#include "StringUtil.h"

using namespace SocketUtil;

UDPSocket::UDPSocket(SOCKET s) :
	m_socket{ s }
{}

UDPSocket::~UDPSocket()
{
	closesocket(m_socket);
}

bool UDPSocket::Bind(const SocketAddress& addr)
{
	if (bind(m_socket, &addr.GetData(), static_cast<int>(addr.GetSize())) != 0)
	{
		ReportError(L"UDPSocket::Bind");
		return false;
	}

	return true;
}

bool UDPSocket::SendTo(const void* send, size_t length, const SocketAddress& address, int32_t* sentBytes)
{
	int bytes = sendto(m_socket, 
		static_cast<const char*>(send), static_cast<int>(length), 0,
		&address.GetData(), static_cast<int>(address.GetSize()));
	if (bytes <= 0)
	{
		ReportError(L"UDPSocket::SendTo");
		return false;
	}

	if (sentBytes != nullptr) (*sentBytes) = bytes;

	return true;
}

bool UDPSocket::ReceiveFrom(void* receive, size_t maxLength, SocketAddress& outFromAddress, int32_t* recvBytes)
{
	socklen_t fromLength = static_cast<socklen_t>(outFromAddress.GetSize());

	auto receiveBytes = recvfrom(m_socket, 
		static_cast<char*>(receive), static_cast<int>(maxLength), 0, 
		&outFromAddress.GetData(), &fromLength);
	if (receiveBytes < 0)
	{
		int error = GetLastError();
		switch (error)
		{
		case WSAEWOULDBLOCK:	return true;
		case WSAECONNRESET:
			StringUtils::Log(L"Connection reset from %s", outFromAddress.ToString().c_str());
			return true;
		default:
			return false;
		}
	}

	if (recvBytes != nullptr) (*recvBytes) = receiveBytes;

	return true;
}