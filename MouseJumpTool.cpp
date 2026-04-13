#include "MouseJumpTool.h"
#include <string>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "gdi32.lib")

const wchar_t g_szMutexName[] = L"MouseJumpTool_Unique_7B2F1C3D";

// ==============================================
// 主工具类
// ==============================================
MouseJumpTool::MouseJumpTool(HINSTANCE hInst)
    : m_hInst(hInst)
    , m_currentMouseScreen(1)
    , m_pMainWnd(nullptr)
    , m_pHintWnd(nullptr)
    , m_pIndicatorWnd(nullptr)
    , m_hMarkOverlayWnd(NULL)
{
}

bool MouseJumpTool::CheckSingleInstance() {
    HANDLE hMutex = CreateMutexW(NULL, TRUE, g_szMutexName);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hMutex);
        MessageBoxW(NULL, L"工具已运行", L"提示", MB_OK | MB_ICONINFORMATION);
        return false;
    }
    return true;
}

bool MouseJumpTool::Init() {
    SetProcessDPIAware();
    EnumDisplayMonitors(NULL, NULL, [](HMONITOR, HDC, LPRECT rc, LPARAM p) -> BOOL {
        auto* tool = reinterpret_cast<MouseJumpTool*>(p);
        tool->m_monitors.push_back(*rc);
        return TRUE;
        }, reinterpret_cast<LPARAM>(this));

    if (m_monitors.empty()) return false;

    m_pMainWnd = new MainWindow(this);
    m_pHintWnd = new HintWindow(this);
    m_pIndicatorWnd = new IndicatorWindow(this);

    if (!m_pMainWnd->Create()) return false;
    if (!m_pHintWnd->Create()) return false;
    if (!m_pIndicatorWnd->Create()) return false;

    SetTimer(m_pMainWnd->GetHandle(), TIMER_MONITOR_MOUSE, 100, NULL);
    return true;
}

bool MouseJumpTool::Run() {
    if (!Init()) return false;
    MessageLoop();

    delete m_pMainWnd;
    delete m_pHintWnd;
    delete m_pIndicatorWnd;
    return true;
}

void MouseJumpTool::MessageLoop() {
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// ==============================================
// 方案二：稳定自绘屏标（无闪烁、双击秒出、3秒消失）
// ==============================================
void MouseJumpTool::ShowAllScreenMarks() {
    HideAllScreenMarks();

    // 全屏透明 overlay 窗口
    m_hMarkOverlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"STATIC", NULL, WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, m_hInst, NULL);

    SetLayeredWindowAttributes(m_hMarkOverlayWnd, 0, 0, LWA_ALPHA); // 全透明
    ShowWindow(m_hMarkOverlayWnd, SW_SHOWNOACTIVATE);

    HDC hdc = GetDC(m_hMarkOverlayWnd);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 60, 60)); // 系统红

    for (size_t i = 0; i < m_monitors.size(); ++i) {
        RECT rc = m_monitors[i];
        int x = rc.left + (rc.right - rc.left) / 2 - 80;
        int y = rc.top + (rc.bottom - rc.top) / 2 - 100;

        HFONT hFont = CreateFontW(
            160, 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        TextOutW(hdc, x, y, std::to_wstring(i + 1).c_str(), 1);
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
    }
    ReleaseDC(m_hMarkOverlayWnd, hdc);
    SetTimer(m_pMainWnd->GetHandle(), TIMER_HIDE_MARKS, 3000, NULL);
}

void MouseJumpTool::HideAllScreenMarks() {
    if (m_hMarkOverlayWnd) {
        KillTimer(m_pMainWnd->GetHandle(), TIMER_HIDE_MARKS);
        DestroyWindow(m_hMarkOverlayWnd);
        m_hMarkOverlayWnd = NULL;
    }
}

//void MouseJumpTool::ShowAllScreenMarks() {
//    if (m_hMarkOverlayWnd) {
//        DestroyWindow(m_hMarkOverlayWnd);
//    }
//
//    m_hMarkOverlayWnd = CreateWindowExW(
//        WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
//        L"STATIC", NULL, WS_POPUP,
//        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
//        NULL, NULL, m_hInst, NULL);
//
//    SetLayeredWindowAttributes(m_hMarkOverlayWnd, 0, 1, LWA_ALPHA);
//    ShowWindow(m_hMarkOverlayWnd, SW_SHOWNOACTIVATE);
//
//    HDC hdc = GetDC(m_hMarkOverlayWnd);
//    for (size_t i = 0; i < m_monitors.size(); ++i) {
//        auto& rc = m_monitors[i];
//        int x = rc.left + (rc.right - rc.left) / 2 - 50;
//        int y = rc.top + (rc.bottom - rc.top) / 2 - 60;
//
//        HFONT font = CreateFontW(90, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei");
//        HFONT old = (HFONT)SelectObject(hdc, font);
//        SetBkMode(hdc, TRANSPARENT);
//        SetTextColor(hdc, RGB(255, 0, 0));
//        TextOutW(hdc, x, y, (L"屏幕 " + std::to_wstring(i + 1)).c_str(), 4);
//        SelectObject(hdc, old);
//        DeleteObject(font);
//    }
//    ReleaseDC(m_hMarkOverlayWnd, hdc);
//
//    SetTimer(m_pMainWnd->GetHandle(), TIMER_HIDE_MARKS, 3000, NULL);
//}
//
//void MouseJumpTool::HideAllScreenMarks() {
//    if (m_hMarkOverlayWnd) {
//        KillTimer(m_pMainWnd->GetHandle(), TIMER_HIDE_MARKS);
//        DestroyWindow(m_hMarkOverlayWnd);
//        m_hMarkOverlayWnd = NULL;
//    }
//}

// ==============================================
// 主窗口
// ==============================================
MainWindow::MainWindow(MouseJumpTool* tool) : m_tool(tool), m_hWnd(NULL) {}

bool MainWindow::Create() {
    WNDCLASSEXW wc{ sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = m_tool->m_hInst;
    wc.lpszClassName = L"MainWindowClass";
    wc.style = CS_DBLCLKS;
    RegisterClassExW(&wc);

    m_hWnd = CreateWindowExW(0, L"MainWindowClass", NULL, 0,
        0, 0, 0, 0, HWND_MESSAGE, NULL, m_tool->m_hInst, this);

    int maxKey = min((int)m_tool->m_monitors.size(), 9);
    for (int i = 0; i < maxKey; ++i)
        RegisterHotKey(m_hWnd, i + 1, MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, '1' + i);

    NOTIFYICONDATAW nid{ sizeof(nid) };
    nid.hWnd = m_hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"鼠标屏幕切换工具");
    Shell_NotifyIconW(NIM_ADD, &nid);

    return m_hWnd != NULL;
}

LRESULT CALLBACK MainWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        auto* pThis = static_cast<MainWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        return 1;
    }
    auto* pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    return pThis ? pThis->HandleMessage(msg, wParam, lParam) : DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT MainWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_HOTKEY:
        m_tool->m_pHintWnd->ShowAt((int)wParam - 1);
        return 0;

    case WM_TIMER:
        if (wParam == TIMER_MONITOR_MOUSE) {
            POINT pt; GetCursorPos(&pt);
            int s = 1;
            for (size_t i = 0; i < m_tool->m_monitors.size(); ++i)
                if (PtInRect(&m_tool->m_monitors[i], pt)) { s = i + 1; break; }
            m_tool->m_currentMouseScreen = s;
            m_tool->m_pIndicatorWnd->UpdateDisplay();
        }
        if (wParam == TIMER_HIDE_MARKS) m_tool->HideAllScreenMarks();
        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_TRAY_EXIT) {
            m_tool->HideAllScreenMarks();
            DestroyWindow(m_hWnd);
        }
        return 0;

    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            POINT pt; GetCursorPos(&pt);
            SetForegroundWindow(m_hWnd);
            HMENU menu = CreatePopupMenu();
            AppendMenuW(menu, MF_STRING, ID_TRAY_EXIT, L"退出程序");
            TrackPopupMenu(menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hWnd, NULL);
            DestroyMenu(menu);
            PostMessage(m_hWnd, WM_NULL, 0, 0);
        }
        if (lParam == WM_LBUTTONDBLCLK) {
            m_tool->ShowAllScreenMarks();
        }
        return 0;

    case WM_DESTROY:
        for (int i = 1; i <= 9; ++i) UnregisterHotKey(m_hWnd, i);
        NOTIFYICONDATAW nid{ sizeof(nid) };
        nid.hWnd = m_hWnd; nid.uID = 1;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}

// ==============================================
// 提示窗口
// ==============================================
HintWindow::HintWindow(MouseJumpTool* tool)
    : m_tool(tool), m_hWnd(NULL), m_remainingSec(0), m_currentScreen(1) {
}

bool HintWindow::Create() {
    WNDCLASSEXW wc{ sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = m_tool->m_hInst;
    wc.lpszClassName = L"HintWindowClass";
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    wc.style = CS_DBLCLKS;
    RegisterClassExW(&wc);

    m_hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        L"HintWindowClass", NULL, WS_POPUP,
        0, 0, 300, 100,
        NULL, NULL, m_tool->m_hInst, this);

    SetLayeredWindowAttributes(m_hWnd, 0, 245, LWA_ALPHA);
    return m_hWnd != NULL;
}

void HintWindow::ShowAt(int idx) {
    const RECT& rc = m_tool->m_monitors[idx];
    int cx = rc.left + (rc.right - rc.left) / 2;
    int cy = rc.top + (rc.bottom - rc.top) / 2;
    SetCursorPos(cx, cy);
    m_currentScreen = idx + 1;
    m_remainingSec = 3;
    SetWindowPos(m_hWnd, HWND_TOPMOST, cx - 150, cy - 50, 300, 100, SWP_SHOWWINDOW | SWP_NOACTIVATE);
    SetTimer(m_hWnd, TIMER_COUNTDOWN, 1000, NULL);
    InvalidateRect(m_hWnd, NULL, TRUE);
}

LRESULT CALLBACK HintWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* pThis = reinterpret_cast<HintWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        pThis = static_cast<HintWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        return 1;
    }
    return pThis ? pThis->HandleMessage(msg, wParam, lParam) : DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT HintWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_PAINT) { PAINTSTRUCT ps; HDC hdc = BeginPaint(m_hWnd, &ps); OnPaint(hdc); EndPaint(m_hWnd, &ps); return 0; }
    if (msg == WM_TIMER) { OnTimer(); return 0; }
    return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}

void HintWindow::OnPaint(HDC hdc) {
    RECT r; GetClientRect(m_hWnd, &r);
    HBRUSH bg = CreateSolidBrush(RGB(255, 255, 0));
    HBRUSH bd = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &r, bg); FrameRect(hdc, &r, bd);
    InflateRect(&r, -2, -2); FrameRect(hdc, &r, bd);

    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(0, 0, 0));
    HFONT f1 = CreateFontW(28, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei");
    HFONT old = (HFONT)SelectObject(hdc, f1);
    RECT r1 = r; r1.bottom -= 30;
    DrawTextW(hdc, (L"屏幕 " + std::to_wstring(m_currentScreen)).c_str(), -1, &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    HFONT f2 = CreateFontW(16, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei");
    SelectObject(hdc, f2);
    RECT r2 = r; r2.top += 40;
    DrawTextW(hdc, (L"关闭 " + std::to_wstring(m_remainingSec) + L"s").c_str(), -1, &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, old); DeleteObject(f1); DeleteObject(f2); DeleteObject(bg); DeleteObject(bd);
}

void HintWindow::OnTimer() {
    if (--m_remainingSec <= 0) { KillTimer(m_hWnd, TIMER_COUNTDOWN); ShowWindow(m_hWnd, SW_HIDE); }
    else InvalidateRect(m_hWnd, NULL, TRUE);
}

// ==============================================
// 悬浮小窗口
// ==============================================
IndicatorWindow::IndicatorWindow(MouseJumpTool* tool)
    : m_tool(tool), m_hWnd(NULL), m_style(IndicatorStyle::RoundRect) {
}

bool IndicatorWindow::Create() {
    WNDCLASSEXW wc{ sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = m_tool->m_hInst;
    wc.lpszClassName = L"IndicatorWindowClass";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.style = CS_DBLCLKS;
    RegisterClassExW(&wc);

    const RECT& main = m_tool->m_monitors[0];
    int x = main.left + 20;
    int y = main.top + 20;
    int w = 120, h = 60;
    if (m_style == IndicatorStyle::Circle) w = h = 80;

    m_hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        L"IndicatorWindowClass", NULL, WS_POPUP,
        x, y, w, h, NULL, NULL, m_tool->m_hInst, this);

    SetLayeredWindowAttributes(m_hWnd, 0, 230, LWA_ALPHA);
    UpdateShape();
    ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    return true;
}

void IndicatorWindow::UpdateDisplay() { InvalidateRect(m_hWnd, NULL, TRUE); }

void IndicatorWindow::UpdateShape() {
    RECT rc; GetClientRect(m_hWnd, &rc);
    HRGN r = (m_style == IndicatorStyle::RoundRect)
        ? CreateRoundRectRgn(0, 0, rc.right, rc.bottom, 18, 18)
        : CreateEllipticRgn(0, 0, rc.right, rc.bottom);
    SetWindowRgn(m_hWnd, r, TRUE);
    DeleteObject(r);
}

void IndicatorWindow::ShowContextMenu() {
    POINT pt; GetCursorPos(&pt);
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, ID_TRAY_EXIT, L"退出程序");

    SetForegroundWindow(m_hWnd);
    TrackPopupMenu(menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_tool->m_pMainWnd->GetHandle(), NULL);
    DestroyMenu(menu);
    PostMessage(m_hWnd, WM_NULL, 0, 0);
}

// ==============================================
// 悬浮小窗口（修复双击+右键）
// ==============================================
LRESULT CALLBACK IndicatorWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* pThis = reinterpret_cast<IndicatorWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        pThis = static_cast<IndicatorWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        return 1;
    }

    // 👇 关键修复：保留拖动 + 正常响应鼠标
    if (msg == WM_NCHITTEST) {
        // 先让系统判断位置
        LRESULT hit = DefWindowProcW(hWnd, msg, wParam, lParam);
        // 只有**左键按下**时才当成标题栏（允许拖动）
        if (hit == HTCLIENT && GetAsyncKeyState(VK_LBUTTON) < 0) {
            return HTCAPTION; // 拖动模式
        }
        return hit; // 正常模式（响应双击/右键）
    }

    return pThis ? pThis->HandleMessage(msg, wParam, lParam) : DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT IndicatorWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hWnd, &ps);
        OnPaint(hdc);
        EndPaint(m_hWnd, &ps);
        return 0;
    }
                 // 👇 原来的客户区消息
    case WM_RBUTTONUP:
    case WM_CONTEXTMENU:
        ShowContextMenu();
        return 0;
    case WM_LBUTTONDBLCLK:
        m_tool->ShowAllScreenMarks();
        return 0;

        // 👇 新增：非客户区消息（兼容拖动时点击）
    case WM_NCRBUTTONUP:
        ShowContextMenu();
        return 0;
    case WM_NCLBUTTONDBLCLK:
        m_tool->ShowAllScreenMarks();
        return 0;
    }
    return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}
void IndicatorWindow::OnPaint(HDC hdc) {
    RECT rc; GetClientRect(m_hWnd, &rc);
    HBRUSH bg = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &rc, bg); DeleteObject(bg);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 0));
    HFONT f = CreateFontW(32, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei");
    HFONT old = (HFONT)SelectObject(hdc, f);
    DrawTextW(hdc, (L"屏幕 " + std::to_wstring(m_tool->m_currentMouseScreen)).c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(f);
}