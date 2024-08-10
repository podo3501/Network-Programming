#include "pch.h"
#include "SocketAddress.h"

SocketAddress::SocketAddress() :
	m_sockAddr{}
{}

SocketAddress::SocketAddress(uint32_t inAddress, uint16_t inPort) :
	m_sockAddr{}
{
	sockaddr_in* addrIn = GetAsSockAddrIn();
	addrIn->sin_family = AF_INET;
	addrIn->sin_addr.S_un.S_addr = htonl(inAddress);
	addrIn->sin_port = htons(inPort);
}

SocketAddress::SocketAddress(const sockaddr& inSockAddr)
{
	memcpy(&m_sockAddr, &inSockAddr, GetSize());
}