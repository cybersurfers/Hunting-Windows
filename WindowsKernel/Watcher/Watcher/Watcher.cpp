#include <Windows.h>
#include <iostream>
#include <string>
#include <algorithm>

#include "Common.h"

std::string GetErrorMessage()
{
    LPSTR messageBuffer = NULL;
    DWORD size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
    );
    std::string error(messageBuffer, size);

    LocalFree(messageBuffer);

    error.erase(std::remove(error.begin(), error.end(), '\n'), error.end());
    return error;
}

int main(int argc, char* argv[])
{
    std::string DriverName = "\\driver\\";

    std::cout << "\n\t--==[[ The Watcher Client ]]==--\n" << std::endl;
    if (argc != 2)
    {
        std::cerr << "[+] Usage: " << argv[0] << " DriverName" << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        DriverName.append(argv[1]);
    }

    HANDLE hFile = CreateFile(
        L"\\\\.\\ch3rn0byl",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::cerr << "[!] CreateFile error: " << GetErrorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    LPVOID lpInBuffer[1024] = { 0 };
    if (DriverName.size() > 1024)
    {
        std::cerr << "[!] Highly doubt you need that big of a name... ;)" << std::endl;
    }
    else
    {
        memcpy(lpInBuffer, DriverName.c_str(), DriverName.size());
    }

    DWORD lpBytesReturned = 0;
    bool status = DeviceIoControl(hFile, DRIVER_HOOK, lpInBuffer, static_cast<DWORD>(DriverName.size()), NULL, NULL, &lpBytesReturned, NULL);
    if (!status)
    {
        std::cerr << "[!] DeviceIoControl error: " << GetErrorMessage() << std::endl;
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }
    std::cout << "[+] Hooked " << DriverName.c_str() << "!" << std::endl;

    CloseHandle(hFile);
    return EXIT_SUCCESS;
}