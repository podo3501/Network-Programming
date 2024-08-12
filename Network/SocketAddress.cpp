#include "pch.h"
#include "SocketAddress.h"
#include "StringUtil.h"
#include "SocketUtil.h"

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

SocketAddress::SocketAddress(const std::string& fullAddr) :
	m_sockAddr{}
{
	std::string host, service;
	SocketUtil::GetHostAndService(fullAddr, &host, &service);

	sockaddr_in* addrIn = GetAsSockAddrIn();
	addrIn->sin_family = AF_INET;
	auto result = inet_pton(AF_INET, host.c_str(), &addrIn->sin_addr);
	addrIn->sin_port = htons(std::stoi(service));
}

std::string SocketAddress::ToString() const
{
	const sockaddr_in* s = GetAsSockAddrIn();
	wchar_t destinationBuffer[16];
	InetNtop(s->sin_family, const_cast<in_addr*>(&s->sin_addr), destinationBuffer, 16);
	return StringUtils::Sprintf("%s:%d",
		destinationBuffer,
		ntohs(s->sin_port));
}