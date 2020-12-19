#include <iostream>
#include <string>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>

void ErrorExit(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                      (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                    TEXT("%s failed with error %d: %s"),
                    lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}

void CenterForegroundWindow(int workWidth, int workHeight)
{
    HWND currentWindow = GetForegroundWindow();
    RECT rect;
    int rectWidth, rectHeight;
    TCHAR windowTitle[256];

    if (!GetWindowRect(currentWindow, &rect))
        ErrorExit(TEXT("GetWindowRect"));

    rectWidth = rect.right - rect.left;
    rectHeight = rect.bottom - rect.top;

    if (!GetWindowText(currentWindow, windowTitle, 256))
    {
        TCHAR msg[256] = TEXT("\nNo window found");
        OutputDebugString(msg);
    }
    else
    {
        TCHAR msg[256] = TEXT("\nFound window: ");
        _tcscat_s(msg, 256, windowTitle);
        OutputDebugString(msg);
    }

    int newX = (workWidth / 2) - (rectWidth / 2);
    int newY = (workHeight / 2) - (rectHeight / 2);

    SetWindowPos(currentWindow, HWND_TOP, newX, newY, rectWidth, rectHeight, SWP_SHOWWINDOW);
}

int main()
{
    int workWidth, workHeight;
    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    workWidth = workArea.right - workArea.left;
    workHeight = workArea.bottom - workArea.top;

    RegisterHotKey(NULL, 0, MOD_WIN | MOD_ALT, 0x43);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_HOTKEY && msg.wParam == 0)
        {
            CenterForegroundWindow(workWidth, workHeight);
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}