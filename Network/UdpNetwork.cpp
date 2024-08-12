#include "pch.h"
#include "UdpNetwork.h"
#include "UDPSocket.h"
#include "SocketAddress.h"
#include "SocketUtil.h"
#include "Utils.h"

using namespace SocketUtil;

std::unique_ptr<UDPProtocol> CreateUDPServer(const std::string& fullAddr)
{
	auto udp = std::make_unique<UdpNetwork>();
	if (!udp->Bind(fullAddr))
		return nullptr;

	return std::move(udp);
}

std::unique_ptr<UDPProtocol> CreateUDPClient()
{
	auto udp = std::make_unique<UdpNetwork>();
	if (!udp->CreateSocket())
		return nullptr;

	return std::move(udp);
}

UdpNetwork::UdpNetwork() :
	m_udpSocket{ nullptr }
{}

UdpNetwork::~UdpNetwork()
{
	Cleanup();
}

bool UdpNetwork::CreateSocket()
{
	ReturnIfFalse(Startup());

	m_udpSocket = CreateUDPSocket(AF_INET);
	if (m_udpSocket == nullptr) return false;

	return true;
}

bool UdpNetwork::Bind(const std::string& fullAddr)
{
	ReturnIfFalse(CreateSocket());

	sockaddr addr{};
	ReturnIfFalse(GetAddressInfo(fullAddr, &addr));

	SocketAddress sockAddress(addr);
	ReturnIfFalse(m_udpSocket->Bind(sockAddress));

	return true;
}

bool UdpNetwork::ReceiveFrom(void* receive, int maxLength, SocketAddress& outFromAddress, int32_t* recvBytes)
{
	if (m_udpSocket == nullptr) return false;

	m_udpSocket->ReceiveFrom(receive, maxLength, outFromAddress, recvBytes);

	return true;
}

bool UdpNetwork::SendTo(const void* send, size_t length, const SocketAddress& address, int32_t* sentBytes)
{
	if (m_udpSocket == nullptr) return false;

	m_udpSocket->SendTo(send, length, address, sentBytes);

	return true;
}