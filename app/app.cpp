#include <tchar.h>
#include <windows.h>
#include <psapi.h>
#include <iostream>

using namespace std;

int main() {
    HWND hwndMinesweeper = FindWindow(NULL, _T("Minesweeper"));
    cout << hwndMinesweeper << endl;
    if (hwndMinesweeper) {
        RECT rect;
        if (GetClientRect(hwndMinesweeper, &rect)) {
            cout << rect.left << " " << rect.top << " " << rect.right << " " << rect.bottom << endl;
            cout << rect.right - rect.left << " x " << rect.bottom - rect.top << endl;
        }
        DWORD dwProcessId;
        DWORD dwThreadId = GetWindowThreadProcessId(hwndMinesweeper, &dwProcessId);
        cout << dwProcessId << endl;
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
        cout << hProcess << endl;
        cout << GetLastError() << endl;
        TCHAR szModuleName[1024];
        UINT cch = sizeof(szModuleName) / sizeof(szModuleName[0]);
        cout << cch << endl;
        cch = GetModuleFileNameEx(hProcess, NULL, (LPTSTR) &szModuleName, cch);
        cout << cch << endl;
        cout << GetLastError() << endl;
        wcout << szModuleName << endl;
        HDC hWinDC = GetDC(hwndMinesweeper);
        HDC hBmpDC = CreateCompatibleDC(hWinDC);
        HBITMAP hBitmap = CreateCompatibleBitmap(hWinDC, rect.right - rect.left, rect.bottom - rect.top);
        SelectObject(hBmpDC, hBitmap);
        PrintWindow(hwndMinesweeper, hBmpDC, PW_CLIENTONLY);
        //BitBlt(hBmpDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hWinDC, 0, 0, SRCCOPY);
        ReleaseDC(hwndMinesweeper, hWinDC);

        BITMAP bitmap;
        GetObject(hBitmap, sizeof(BITMAP), &bitmap);
        BITMAPINFOHEADER bmpInfoHdr;
        bmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfoHdr.biWidth = bitmap.bmWidth;
        bmpInfoHdr.biHeight = bitmap.bmHeight;
        bmpInfoHdr.biPlanes = 1;
        bmpInfoHdr.biBitCount = 32;
        bmpInfoHdr.biCompression = BI_RGB;
        bmpInfoHdr.biSizeImage = 0;
        bmpInfoHdr.biXPelsPerMeter = 0;
        bmpInfoHdr.biYPelsPerMeter = 0;
        bmpInfoHdr.biClrUsed = 0;
        bmpInfoHdr.biClrImportant = 0;
        DWORD dwBmpSize = ((bitmap.bmWidth * bmpInfoHdr.biBitCount + 31) / 32) * 4 * bitmap.bmHeight;
        char* lpBitmap = new char[dwBmpSize];
        GetDIBits(hBmpDC, hBitmap, 0, (UINT)bitmap.bmHeight, lpBitmap, (BITMAPINFO*)&bmpInfoHdr, DIB_RGB_COLORS);
        DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        BITMAPFILEHEADER bmpFileHdr;
        bmpFileHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
        bmpFileHdr.bfSize = dwSizeofDIB;
        bmpFileHdr.bfType = 0x4D42; // BM.
        HANDLE hFile = CreateFile(L"minesweeper.bmp",
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD dwBytesWritten;
        WriteFile(hFile, (LPSTR)&bmpFileHdr, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
        WriteFile(hFile, (LPSTR)&bmpInfoHdr, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
        WriteFile(hFile, (LPSTR)lpBitmap, dwBmpSize, &dwBytesWritten, NULL);
        CloseHandle(hFile);
        DeleteObject(hBitmap);
        DeleteObject(hBmpDC);
    }
}
