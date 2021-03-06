#include "pch.h"

std::default_random_engine dre{ std::random_device{}() };

#pragma region ERROR
void ErrorQuit(std::wstring_view msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, nullptr);

	MessageBox(nullptr, static_cast<LPCTSTR>(lpMsgBuf), msg.data(), MB_ICONERROR);

	LocalFree(lpMsgBuf);
	exit(true);
}

void DisplayError(std::wstring_view msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, nullptr);

	std::wcout << msg << " : " << static_cast<LPCTSTR>(lpMsgBuf) << std::endl;

	LocalFree(lpMsgBuf);
}
#pragma endregion