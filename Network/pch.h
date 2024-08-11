#pragma once

#define WIN32_LEAN_AND_MEAN

#pragma comment (lib, "Ws2_32.lib")

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <assert.h>
#include <string>
#include <memory>
#include <iostream>
