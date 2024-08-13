#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <array>
#include <string>
#include "../Include/NetworkInterface.h"

#pragma comment (lib, "Ws2_32.lib")

constexpr int DEFAULT_BUFLEN = 512;

//�ܼ�â�� ���� ������ ��� �������ᰡ �ƴ϶� �޸𸮰� ���� �κ��� �����. 
//�̷��� �ϸ� �����Ҷ� �� �κ��� ���İ��� �Ǿ� �Ҹ��ڰ� ȣ��ǰ� �ȴ�.
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT: //CTRL + C
	case CTRL_CLOSE_EVENT: //Close Action
	case CTRL_BREAK_EVENT: //CTRL + Pause Break
	case CTRL_LOGOFF_EVENT: //System Logoff
	case CTRL_SHUTDOWN_EVENT: //System power off
		break;
	default: 
		break;
	}
	return TRUE;
}

int __cdecl main(void)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	SetConsoleCtrlHandler(CtrlHandler, TRUE);

	std::unique_ptr<TCPProtocol> tcpServer = CreateTCPProtocol();

	auto result = tcpServer->Setup(HostType::Server, "192.168.0.125:27005");
	if (result != true)
		return 1;

	std::cout << "Connected" << std::endl;

	int32_t recvBytes{ 0 };
	do
	{
		std::array<void*, DEFAULT_BUFLEN> recvbuf{};
		tcpServer->Receive(recvbuf.data(), recvbuf.size(), &recvBytes);
		if (recvBytes > 0)
		{
			std::cout << "Bytes received: " << recvBytes << std::endl;
			tcpServer->Send(recvbuf.data(), recvBytes, nullptr);
		}
	} while (recvBytes > 0);

	std::cout << "Shutdown" << std::endl;
	tcpServer->Shutdown();

	return 0;
}