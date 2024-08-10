#pragma once

class TCPSocket;

namespace SocketUtil
{
	void ReportError(const std::wstring errorDesc);

	std::unique_ptr<TCPSocket> CreateTCPSocket(int addrFamily);
}