#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <array>
#include "../Include/NetworkInterface.h"

constexpr int DEFAULT_BUFLEN = 512;

//콘솔창을 강제 종료할 경우 정상종료가 아니라 메모리가 세는 부분이 생긴다. 
//이렇게 하면 종료할때 이 부분을 거쳐가게 되어 소멸자가 호출되게 된다.
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

int __cdecl main(int argc, char** argv)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	SetConsoleCtrlHandler(CtrlHandler, TRUE);

	std::unique_ptr<Client> network = CreateClient();

	auto result = network->Setup("192.168.0.125:27015");
	if (result != true)
		return 1;

	result = network->Connect();
	if (result != true)
		return 1;

	std::string sendbuf{ "this is a test" };
	result = network->Send(sendbuf.c_str(), sendbuf.size(), nullptr);
	if (result != true)
		return 1;

	network->Shutdown();

	int32_t recvBytes{ 0 };
	do
	{
		std::array<void*, DEFAULT_BUFLEN> recvbuf{};
		result = network->Receive(recvbuf.data(), recvbuf.size(), &recvBytes);
		if (recvBytes > 0)
			std::cout << "Bytes received: " << recvBytes << std::endl;
	} while (recvBytes > 0);

	system("pause");

	return 0;
}