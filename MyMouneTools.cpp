#include <Windows.h>
#include <vector>
#include <string>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "gdi32.lib")


#include "MouseJumpTool.h"

// 程序主入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
	// 检查是否已运行
	if (!MouseJumpTool::CheckSingleInstance()) {
		return 0;
	}

	// 创建工具对象并运行
	MouseJumpTool tool(hInstance);
	tool.Run();

	return 0;
}


//#define WM_TRAYICON      (WM_USER + 100)
//#define ID_TRAY_EXIT     1001
//#define TIMER_COUNTDOWN  2003
//
//using namespace std;
//
//static vector<RECT> g_monitors;
//static NOTIFYICONDATAW g_nid = { 0 };
//static HWND g_hHintWnd = NULL;
//static int g_remainingSec = 0;
//static int g_currentIdx = 0;
//
//// 唯一互斥体名称，保证系统内唯一
//const wchar_t g_szUniqueMutex[] = L"MouseJumpTool_UniqueInstance_{7B2F1C3D-1A2E-4F5A-9B8C-7D6E5F4A3B2C}";
//
//BOOL CALLBACK MonitorEnumProc(HMONITOR, HDC, LPRECT lprcMonitor, LPARAM) {
//    g_monitors.emplace_back(*lprcMonitor);
//    return TRUE;
//}
//
//LRESULT CALLBACK HintWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//    switch (msg) {
//        case WM_PAINT: {
//            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hWnd, &ps);
//            RECT rect;
//            GetClientRect(hWnd, &rect);
//
//            HBRUSH hBgBrush = CreateSolidBrush(RGB(255, 255, 0));
//            FillRect(hdc, &rect, hBgBrush);
//            HBRUSH hBorderBrush = CreateSolidBrush(RGB(0, 0, 0));
//            FrameRect(hdc, &rect, hBorderBrush);
//            InflateRect(&rect, -3, -3);
//            FrameRect(hdc, &rect, hBorderBrush);
//
//            DeleteObject(hBgBrush);
//            DeleteObject(hBorderBrush);
//
//            SetBkMode(hdc, TRANSPARENT);
//            SetTextColor(hdc, RGB(0, 0, 0));
//
//            HFONT hTitleFont = CreateFontW(54, 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE,
//                DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei");
//            HFONT hOldFont = (HFONT)SelectObject(hdc, hTitleFont);
//
//            wstring title = L"鼠标已切换到屏幕 " + to_wstring(g_currentIdx);
//            RECT rcTitle = rect;
//            rcTitle.bottom -= 50;
//            DrawTextW(hdc, title.c_str(), -1, &rcTitle, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//
//            HFONT hCountFont = CreateFontW(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
//                DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei");
//            SelectObject(hdc, hCountFont);
//
//            RECT rcCount = rect;
//            rcCount.top += 90;
//            wstring count = L"此提示将在 " + to_wstring(g_remainingSec) + L" 秒后自动关闭";
//            DrawTextW(hdc, count.c_str(), -1, &rcCount, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//
//            SelectObject(hdc, hOldFont);
//            DeleteObject(hTitleFont);
//            DeleteObject(hCountFont);
//
//            EndPaint(hWnd, &ps);
//            return 0;
//        }
//        case WM_TIMER: {
//            if (wParam != TIMER_COUNTDOWN) break;
//            if (--g_remainingSec <= 0) {
//                KillTimer(hWnd, TIMER_COUNTDOWN);
//                ShowWindow(hWnd, SW_HIDE);
//            } else {
//                InvalidateRect(hWnd, NULL, TRUE);
//            }
//            return 0;
//        }
//        default:
//            return DefWindowProcW(hWnd, msg, wParam, lParam);
//    }
//}
//
//void MoveMouseAndShowHint(int idx) {
//    if (idx < 0 || idx >= (int)g_monitors.size()) return;
//
//    g_currentIdx = idx + 1;
//    const RECT& rc = g_monitors[idx];
//    int cx = rc.left + (rc.right - rc.left) / 2;
//    int cy = rc.top + (rc.bottom - rc.top) / 2;
//
//    SetCursorPos(cx, cy);
//
//    g_remainingSec = 3;
//    KillTimer(g_hHintWnd, TIMER_COUNTDOWN);
//
//    const int W = 600, H = 200;
//    SetWindowPos(g_hHintWnd, HWND_TOPMOST,
//        cx - W / 2, cy - H / 2, W, H,
//        SWP_SHOWWINDOW | SWP_NOACTIVATE);
//
//    InvalidateRect(g_hHintWnd, NULL, TRUE);
//    SetTimer(g_hHintWnd, TIMER_COUNTDOWN, 1000, NULL);
//}
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//    switch (msg) {
//        case WM_HOTKEY:
//            MoveMouseAndShowHint((int)wParam - 1);
//            break;
//
//        case WM_TRAYICON:
//            if (LOWORD(lParam) == WM_RBUTTONUP) {
//                POINT pt;
//                GetCursorPos(&pt);
//                HMENU hMenu = CreatePopupMenu();
//                AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"退出程序");
//                SetForegroundWindow(hWnd);
//                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
//                DestroyMenu(hMenu);
//            }
//            break;
//
//        case WM_COMMAND:
//            if (LOWORD(wParam) == ID_TRAY_EXIT)
//                DestroyWindow(hWnd);
//            break;
//
//        case WM_DESTROY:
//            for (int i = 1; i <= 9; ++i)
//                UnregisterHotKey(hWnd, i);
//            Shell_NotifyIconW(NIM_DELETE, &g_nid);
//            PostQuitMessage(0);
//            break;
//
//        default:
//            return DefWindowProcW(hWnd, msg, wParam, lParam);
//    }
//    return 0;
//}
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
//    // ====================== 单实例判断 ======================
//    HANDLE hMutex = CreateMutexW(NULL, TRUE, g_szUniqueMutex);
//    if (GetLastError() == ERROR_ALREADY_EXISTS) {
//        CloseHandle(hMutex);
//        MessageBoxW(NULL, L"鼠标跨屏工具已在运行中", L"提示", MB_OK | MB_ICONINFORMATION);
//        return 0;
//    }
//
//    SetProcessDPIAware();
//    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
//
//    WNDCLASSEXW wcMain = { sizeof(WNDCLASSEXW) };
//    wcMain.lpfnWndProc = WndProc;
//    wcMain.hInstance = hInstance;
//    wcMain.lpszClassName = L"MouseJumpCoreWnd";
//    RegisterClassExW(&wcMain);
//
//    HWND hMainWnd = CreateWindowExW(0, L"MouseJumpCoreWnd", NULL, 0,
//        0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
//
//    WNDCLASSEXW wcHint = { sizeof(WNDCLASSEXW) };
//    wcHint.lpfnWndProc = HintWndProc;
//    wcHint.hInstance = hInstance;
//    wcHint.lpszClassName = L"MouseJumpHintWnd";
//    wcHint.hbrBackground = GetSysColorBrush(COLOR_INFOBK);
//    RegisterClassExW(&wcHint);
//
//    g_hHintWnd = CreateWindowExW(
//        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
//        L"MouseJumpHintWnd", NULL, WS_POPUP,
//        0, 0, 0, 0, NULL, NULL, hInstance, NULL);
//    SetLayeredWindowAttributes(g_hHintWnd, 0, 245, LWA_ALPHA);
//
//    int maxKey = min((int)g_monitors.size(), 9);
//    for (int i = 0; i < maxKey; ++i) {
//        RegisterHotKey(hMainWnd, i + 1, MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, '1' + i);
//    }
//
//    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
//    g_nid.hWnd = hMainWnd;
//    g_nid.uID = 1;
//    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
//    g_nid.uCallbackMessage = WM_TRAYICON;
//    g_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//    wcscpy_s(g_nid.szTip, L"鼠标跨屏工具 Ctrl+Shift+数字");
//    Shell_NotifyIconW(NIM_ADD, &g_nid);
//
//    MSG msg;
//    while (GetMessageW(&msg, NULL, 0, 0)) {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    CloseHandle(hMutex);
//    return 0;
//}