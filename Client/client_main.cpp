#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <array>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

constexpr int DEFAULT_BUFLEN = 512;
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char** argv)
{
	auto iResult{ 0 };

	argc = 2;
	std::string url{ "192.168.0.125" };
	argv[1] = const_cast<char*>(url.c_str());

	if (argc != 2)
	{
		std::cout << "usage: " << argv[0] << " server-name" << std::endl;
		return 1;
	}

	WSADATA wsaData{};
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed with error: " << iResult << std::endl;
		return 1;
	}

	addrinfo* result = nullptr, * ptr = nullptr, hints{};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cout << "getaddrinfo failed with error: " << iResult << std::endl;
		WSACleanup();
		return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;
	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
	{
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return 1;
		}

		iResult = connect(ConnectSocket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		std::cout << "Unable to Connect to server! " << std::endl;
		WSACleanup();
		return 1;
	}

	std::string sendbuf{ "this is a test" };
	iResult = send(ConnectSocket, sendbuf.c_str(), static_cast<int>(sendbuf.size()), 0);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "send failed with error " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Bytes Sent: " << iResult << std::endl;

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	std::array<char, DEFAULT_BUFLEN> recvbuf{};
	do
	{
		iResult = recv(ConnectSocket, recvbuf.data(), static_cast<int>(recvbuf.size()), 0);
		if (iResult > 0)
			std::cout << "Bytes received " << iResult << std::endl;
		else if (iResult == 0)
			std::cout << "Connection closed" << std::endl;
		else
			std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
	} while (iResult > 0);

	closesocket(ConnectSocket);
	WSACleanup();

	system("pause");

	return 0;
}