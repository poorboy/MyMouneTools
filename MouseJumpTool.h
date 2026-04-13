#pragma once
#include <Windows.h>
#include <vector>

// 消息ID定义
#define WM_TRAYICON          (WM_USER + 100)
#define ID_TRAY_EXIT         1001
#define TIMER_COUNTDOWN      2003
#define TIMER_MONITOR_MOUSE  2004
#define TIMER_HIDE_MARKS     2005

// 悬浮小窗口样式
enum class IndicatorStyle {
    RoundRect,  // 圆角矩形
    Circle      // 圆形
};

// 前置声明
class MainWindow;
class HintWindow;
class IndicatorWindow;

// ==============================================
// 鼠标跨屏工具主类
// ==============================================
class MouseJumpTool final {
    friend class MainWindow;
    friend class HintWindow;
    friend class IndicatorWindow;

public:
    explicit MouseJumpTool(HINSTANCE hInst);
    static bool CheckSingleInstance();
    bool Run();

private:
    bool Init();
    void MessageLoop();
    void ShowAllScreenMarks();
    void HideAllScreenMarks();

private:
    HINSTANCE              m_hInst;
    std::vector<RECT>      m_monitors;
    int                    m_currentMouseScreen;

    MainWindow* m_pMainWnd;
    HintWindow* m_pHintWnd;
    IndicatorWindow* m_pIndicatorWnd;

    HWND                   m_hMarkOverlayWnd;
};

// ==============================================
// 窗口1：主窗口（热键 + 托盘）
// ==============================================
class MainWindow {
public:
    explicit MainWindow(MouseJumpTool* tool);
    bool Create();
    HWND GetHandle() const { return m_hWnd; }

private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT HandleMessage(UINT, WPARAM, LPARAM);

private:
    MouseJumpTool* m_tool;
    HWND           m_hWnd;
};

// ==============================================
// 窗口2：跳转提示窗口
// ==============================================
class HintWindow {
public:
    explicit HintWindow(MouseJumpTool* tool);
    bool Create();
    void ShowAt(int screenIdx);

private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT HandleMessage(UINT, WPARAM, LPARAM);
    void OnPaint(HDC hdc);
    void OnTimer();

private:
    MouseJumpTool* m_tool;
    HWND           m_hWnd;
    int            m_remainingSec;
    int            m_currentScreen;
};

// ==============================================
// 窗口3：悬浮小指示器（右键+双击+拖动）
// ==============================================
class IndicatorWindow {
public:
    explicit IndicatorWindow(MouseJumpTool* tool);
    bool Create();
    void UpdateDisplay();

private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT HandleMessage(UINT, WPARAM, LPARAM);
    void OnPaint(HDC hdc);
    void UpdateShape();
    void ShowContextMenu();

private:
    MouseJumpTool* m_tool;
    HWND             m_hWnd;
    IndicatorStyle   m_style;
};