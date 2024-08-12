#pragma once

class SocketAddress
{
public:
	SocketAddress();
	SocketAddress(uint32_t inAddress, uint16_t inPort);
	SocketAddress(const sockaddr& inSockAddr);
	SocketAddress(const std::string& fullAddr);

	SocketAddress(const SocketAddress&) = delete;
	SocketAddress& operator=(const SocketAddress&) = delete;

	sockaddr& GetData() { return m_sockAddr; }
	const sockaddr& GetData() const { return m_sockAddr; }
	size_t GetSize() const { return sizeof(sockaddr); }

	std::string ToString() const;

private:
	sockaddr_in* GetAsSockAddrIn() { return reinterpret_cast<sockaddr_in*>(&m_sockAddr); }
	const sockaddr_in* GetAsSockAddrIn() const { return reinterpret_cast<const sockaddr_in*>(&m_sockAddr); }

	sockaddr m_sockAddr;
};
