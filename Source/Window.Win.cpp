#ifdef _WIN32
#include <algorithm>
#include <Windows.h>
#include "Window.h"
#include "Application.h"

using namespace tk;

thread_local char BUFFER1[4096];
thread_local char BUFFER2[4096];

const TCHAR* ToNative(const std::string& str)
{
    memset(BUFFER1, 0, sizeof(BUFFER1));
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (wchar_t*)BUFFER1, sizeof(BUFFER1));
#ifdef UNICODE
    return (wchar_t*)BUFFER1;
#else
    memset(BUFFER2, 0, sizeof(BUFFER2));
    WideCharToMultiByte(CP_ACP, 0, (wchar_t*)BUFFER1, -1, (char*)BUFFER2, sizeof(BUFFER2), NULL, NULL);
    return (char*)BUFFER2;
#endif
}

std::string FromNative(const TCHAR* str)
{
#ifdef UNICODE
    memset(BUFFER1, 0, sizeof(BUFFER1));
    WideCharToMultiByte(CP_UTF8, 0, str, -1, (char*)BUFFER1, sizeof(BUFFER1), NULL, NULL);
#else
    memset(BUFFER1, 0, sizeof(BUFFER1));
    MultiByteToWideChar(CP_ACP, 0, str, -1, (wchar_t*)BUFFER1, sizeof(BUFFER1));
    memset(BUFFER2, 0, sizeof(BUFFER2));
    WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)BUFFER1, -1, (char*)BUFFER2, sizeof(BUFFER2), NULL, NULL);
#endif
    return std::string(BUFFER2);
}

namespace tk
{
void RunLoop(Application* app, Window* win);

struct NativeWindow
{
    HWND hWnd;

    NativeWindow(Window* win, HWND hWnd)
        : hWnd(hWnd)
    {
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)win);
    }

    ~NativeWindow()
    {
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR) nullptr);
    }
};

struct TranslateKeyModifiers
{
    int m_vk;
    ModifierKey m_modifier;
};

static const TranslateKeyModifiers s_translateKeyModifiers[8] =
    {
        {VK_LMENU, ModifierKey::LeftAlt},
        {VK_RMENU, ModifierKey::RightAlt},
        {VK_LCONTROL, ModifierKey::LeftCtrl},
        {VK_RCONTROL, ModifierKey::RightCtrl},
        {VK_LSHIFT, ModifierKey::LeftShift},
        {VK_RSHIFT, ModifierKey::RightShift},
        {VK_LWIN, ModifierKey::LeftMeta},
        {VK_RWIN, ModifierKey::RightMeta},
};

static uint8_t translateKeyModifiers()
{
    uint8_t modifiers = 0;
    for (uint32_t ii = 0; ii < sizeof(s_translateKeyModifiers) / sizeof(TranslateKeyModifiers); ++ii)
    {
        const TranslateKeyModifiers& tkm = s_translateKeyModifiers[ii];
        modifiers |= 0 > GetKeyState(tkm.m_vk) ? (uint8_t)tkm.m_modifier : (uint8_t)ModifierKey::None;
    }
    return modifiers;
}

Keys translateKey(uint8_t keyCode)
{
    switch (keyCode)
    {
        case VK_ESCAPE:
            return Keys::Esc;
        case VK_RETURN:
            return Keys::Return;
        case VK_TAB:
            return Keys::Tab;
        case VK_BACK:
            return Keys::Backspace;
        case VK_SPACE:
            return Keys::Space;
        case VK_UP:
            return Keys::Up;
        case VK_DOWN:
            return Keys::Down;
        case VK_LEFT:
            return Keys::Left;
        case VK_RIGHT:
            return Keys::Right;
        case VK_INSERT:
            return Keys::Insert;
        case VK_DELETE:
            return Keys::Delete;
        case VK_HOME:
            return Keys::Home;
        case VK_END:
            return Keys::End;
        case VK_PRIOR:
            return Keys::PageUp;
        case VK_NEXT:
            return Keys::PageDown;
        case VK_SNAPSHOT:
            return Keys::Print;
        case VK_OEM_PLUS:
            return Keys::Plus;
        case VK_OEM_MINUS:
            return Keys::Minus;
        case VK_OEM_4:
            return Keys::LeftBracket;
        case VK_OEM_6:
            return Keys::RightBracket;
        case VK_OEM_1:
            return Keys::Semicolon;
        case VK_OEM_7:
            return Keys::Quote;
        case VK_OEM_COMMA:
            return Keys::Comma;
        case VK_OEM_PERIOD:
            return Keys::Period;
        case VK_DECIMAL:
            return Keys::Period;
        case VK_OEM_2:
            return Keys::Slash;
        case VK_OEM_5:
            return Keys::Backslash;
        case VK_OEM_3:
            return Keys::Tilde;
        case VK_F1:
            return Keys::F1;
        case VK_F2:
            return Keys::F2;
        case VK_F3:
            return Keys::F3;
        case VK_F4:
            return Keys::F4;
        case VK_F5:
            return Keys::F5;
        case VK_F6:
            return Keys::F6;
        case VK_F7:
            return Keys::F7;
        case VK_F8:
            return Keys::F8;
        case VK_F9:
            return Keys::F9;
        case VK_F10:
            return Keys::F10;
        case VK_F11:
            return Keys::F11;
        case VK_F12:
            return Keys::F12;
        case VK_NUMPAD0:
            return Keys::NumPad0;
        case VK_NUMPAD1:
            return Keys::NumPad1;
        case VK_NUMPAD2:
            return Keys::NumPad2;
        case VK_NUMPAD3:
            return Keys::NumPad3;
        case VK_NUMPAD4:
            return Keys::NumPad4;
        case VK_NUMPAD5:
            return Keys::NumPad5;
        case VK_NUMPAD6:
            return Keys::NumPad6;
        case VK_NUMPAD7:
            return Keys::NumPad7;
        case VK_NUMPAD8:
            return Keys::NumPad8;
        case VK_NUMPAD9:
            return Keys::NumPad9;
        case uint8_t('0'):
            return Keys::Key0;
        case uint8_t('1'):
            return Keys::Key1;
        case uint8_t('2'):
            return Keys::Key2;
        case uint8_t('3'):
            return Keys::Key3;
        case uint8_t('4'):
            return Keys::Key4;
        case uint8_t('5'):
            return Keys::Key5;
        case uint8_t('6'):
            return Keys::Key6;
        case uint8_t('7'):
            return Keys::Key7;
        case uint8_t('8'):
            return Keys::Key8;
        case uint8_t('9'):
            return Keys::Key9;
        case uint8_t('A'):
            return Keys::KeyA;
        case uint8_t('B'):
            return Keys::KeyB;
        case uint8_t('C'):
            return Keys::KeyC;
        case uint8_t('D'):
            return Keys::KeyD;
        case uint8_t('E'):
            return Keys::KeyE;
        case uint8_t('F'):
            return Keys::KeyF;
        case uint8_t('G'):
            return Keys::KeyG;
        case uint8_t('H'):
            return Keys::KeyH;
        case uint8_t('I'):
            return Keys::KeyI;
        case uint8_t('J'):
            return Keys::KeyJ;
        case uint8_t('K'):
            return Keys::KeyK;
        case uint8_t('L'):
            return Keys::KeyL;
        case uint8_t('M'):
            return Keys::KeyM;
        case uint8_t('N'):
            return Keys::KeyN;
        case uint8_t('O'):
            return Keys::KeyO;
        case uint8_t('P'):
            return Keys::KeyP;
        case uint8_t('Q'):
            return Keys::KeyQ;
        case uint8_t('R'):
            return Keys::KeyR;
        case uint8_t('S'):
            return Keys::KeyS;
        case uint8_t('T'):
            return Keys::KeyT;
        case uint8_t('U'):
            return Keys::KeyU;
        case uint8_t('V'):
            return Keys::KeyV;
        case uint8_t('W'):
            return Keys::KeyW;
        case uint8_t('X'):
            return Keys::KeyX;
        case uint8_t('Y'):
            return Keys::KeyY;
        case uint8_t('Z'):
            return Keys::KeyZ;
    }
    return Keys::None;
}

void DispatchEvent(Window* win, Event* e)
{
    win->OnEvent(e);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CREATE)
    {
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)((LPCREATESTRUCT)lParam)->lpCreateParams);
    }

    auto win = (Window*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    if (win != nullptr)
    {
        LRESULT result = 0;
        if (win->WndProc(msg, wParam, lParam, &result))
            return result;

        switch (msg)
        {
            case WM_SHOWWINDOW:
            {
                Event m;
                m.type = EventType::VisibleChanged;
                DispatchEvent(win, &m);
                break;
            }
            case WM_MOUSEMOVE:
            {
                Event e;
                e.type = EventType::MouseMove;
                DispatchEvent(win, &e);
            }
            break;
            case WM_MOUSEWHEEL:
            {
                MouseWheelEvent e;
                e.type = EventType::MouseWheel;
                e.WheelX = 0;
                e.WheelY = (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
                DispatchEvent(win, &e);
            }
            break;
            case WM_LBUTTONDOWN:
            {
                MouseButtonEvent e;
                e.type = EventType::MouseDown;
                e.Button = MouseButton::Left;
                DispatchEvent(win, &e);
                SetCapture(hWnd);
            }
            break;
            case WM_LBUTTONUP:
            {
                MouseButtonEvent e;
                e.type = EventType::MouseUp;
                e.Button = MouseButton::Left;
                DispatchEvent(win, &e);
                ReleaseCapture();
            }
            break;
            case WM_MBUTTONDOWN:
            {
                MouseButtonEvent e;
                e.type = EventType::MouseDown;
                e.Button = MouseButton::Middle;
                DispatchEvent(win, &e);
                SetCapture(hWnd);
            }
            break;
            case WM_MBUTTONUP:
            {
                MouseButtonEvent e;
                e.type = EventType::MouseUp;
                e.Button = MouseButton::Middle;
                DispatchEvent(win, &e);
                ReleaseCapture();
            }
            break;
            case WM_RBUTTONDOWN:
            {
                MouseButtonEvent e;
                e.type = EventType::MouseDown;
                e.Button = MouseButton::Right;
                DispatchEvent(win, &e);
                SetCapture(hWnd);
            }
            break;
            case WM_RBUTTONUP:
            {
                MouseButtonEvent e;
                e.type = EventType::MouseUp;
                e.Button = MouseButton::Right;
                DispatchEvent(win, &e);
                ReleaseCapture();
            }
            break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                uint8_t modifiers = translateKeyModifiers();
                Keys key = translateKey((uint8_t)wParam);
                if (Keys::Print == key && 0x3 == ((uint32_t)(lParam) >> 30))
                {
                    // VK_SNAPSHOT doesn't generate keydown event. Fire on down event when previous
                    // key state bit is set to 1 and transition state bit is set to 1.
                    //
                    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms646280%28v=vs.85%29.aspx
                    KeyEvent m;
                    m.type = EventType::KeyDown;
                    m.Key = key;
                    m.Modifier = (ModifierKey)modifiers;
                    DispatchEvent(win, &m);
                }
                KeyEvent m;
                m.type = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) ? EventType::KeyDown : EventType::KeyUp;
                m.Key = key;
                m.Modifier = (ModifierKey)modifiers;
                DispatchEvent(win, &m);
                break;
            }
            case WM_CHAR:
            {
                // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
                if (wParam > 0 && wParam < 0x10000)
                {
                    InputEvent m;
                    m.type = EventType::Input;
                    m.Char = (uint32_t)wParam;
                    DispatchEvent(win, &m);
                }
                break;
            }
            case WM_DPICHANGED:
            {
                LPRECT r = (LPRECT)lParam;
                SetWindowPos(hWnd, NULL, r->left, r->top, r->right - r->left, r->bottom - r->top, SWP_NOZORDER | SWP_NOACTIVATE);

                Event e;
                e.type = EventType::DpiChanged;
                DispatchEvent(win, &e);
                break;
            }
            case WM_SIZE:
            {
                Event e;
                e.type = EventType::Resize;
                DispatchEvent(win, &e);
                break;
            }
            case WM_CLOSE:
            {
                Event e;
                e.type = EventType::Closing;
                DispatchEvent(win, &e);
                if (e.result != 0)
                    return 0;
                break;
            }
            case WM_DESTROY:
            {
                if (win->nativeWindow != nullptr)
                {
                    delete win->nativeWindow;
                    win->nativeWindow = nullptr;
                }

                Event e;
                e.type = EventType::Closed;
                DispatchEvent(win, &e);
                if (Application::Current()->GetMainWindow() == win)
                    PostQuitMessage(0);
                break;
            }
        }
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
} // namespace tk

void Window::OnStyleChanged()
{
    if (nativeWindow != NULL)
    {
        auto winStyle = GetWindowLong((HWND)GetHandle(), GWL_STYLE);
        if (style & WINDOW_NOTITLE)
            winStyle &= ~WS_CAPTION;
        else
            winStyle |= WS_CAPTION;

        if (style & WINDOW_BUTTON_MIN)
            winStyle |= WS_MINIMIZEBOX;
        else
            winStyle &= ~WS_MINIMIZEBOX;

        if (style & WINDOW_BUTTON_MAX)
            winStyle |= WS_MAXIMIZEBOX;
        else
            winStyle &= ~WS_MAXIMIZEBOX;

        if (style & WINDOW_RESIZABLE)
            winStyle |= WS_THICKFRAME;
        else
            winStyle &= ~WS_THICKFRAME;

        SetWindowLong((HWND)GetHandle(), GWL_STYLE, winStyle);
        SetWindowPos((HWND)GetHandle(), NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}

bool Window::CreateImpl(Window* parent, std::string title, const Rect<float>& rect)
{
    int32_t win_style = WS_OVERLAPPEDWINDOW;
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, ::WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, TEXT("Window"), NULL};
    RegisterClassEx(&wc);
    float dpi = GetDpiForSystem() / (float)USER_DEFAULT_SCREEN_DPI;
    HWND hWnd = CreateWindowEx(WS_EX_LAYERED, TEXT("Window"), ToNative(title), win_style, (int)(rect.X * dpi), (int)(rect.Y * dpi), (int)(rect.Width * dpi), (int)(rect.Height * dpi), parent == nullptr ? NULL : (HWND)(parent->GetHandle()), NULL, wc.hInstance, this);

    this->nativeWindow = new NativeWindow(this, hWnd);

    Event e;
    e.type = EventType::Create;
    e.result = 0;
    DispatchEvent(this, &e);

    this->OnCreate();

    SetTransparency(1);

    OnStyleChanged();

    return nativeWindow != NULL;
}

void Window::Show()
{
    ShowWindow((HWND)GetHandle(), SW_SHOW);
    UpdateWindow((HWND)GetHandle());
}

void Window::Hide()
{
    ShowWindow((HWND)GetHandle(), SW_HIDE);
}

void Window::Close()
{
    SendMessage((HWND)GetHandle(), WM_CLOSE, 0, 0);
}

void Window::ShowDialog()
{
    HWND parent = (HWND)GetWindowLongPtrW((HWND)GetHandle(), GWLP_HWNDPARENT);
    bool enable = IsWindowEnabled(parent);
    EnableWindow(parent, false);

    Show();

    RunLoop(Application::Current(), this);

    EnableWindow(parent, enable);
    SetForegroundWindow(parent);
}

void* Window::GetHandle() const
{
    if (nativeWindow == nullptr)
        return NULL;

    return nativeWindow->hWnd;
}

float Window::GetDpiScale() const
{
    return GetDpiForWindow((HWND)GetHandle()) / (float)USER_DEFAULT_SCREEN_DPI;
}

Point<float> Window::GetMousePosition() const
{
    POINT point;
    GetCursorPos(&point);
    ScreenToClient((HWND)GetHandle(), &point);
    auto size = GetClientSize();
    float dpi = GetDpiScale();
    return {(float)std::clamp(point.x / dpi, 0.f, (float)size.Width), (float)std::clamp(point.y / dpi, 0.f, (float)size.Height)};
}

void Window::SetMousePosition(const Point<float>& p)
{
    float dpi = GetDpiScale();
    POINT pos = {(int)(p.X * dpi), (int)(p.Y * dpi)};
    if (::ClientToScreen((HWND)GetHandle(), &pos))
        ::SetCursorPos(pos.x, pos.y);
}

void Window::SetCursor(const Cursor& cur)
{
    LPTSTR win32_cursor = IDC_ARROW;
    switch (cur)
    {
        case Cursor::None:
            win32_cursor = NULL;
            break;
        case Cursor::Arrow:
            win32_cursor = IDC_ARROW;
            break;
        case Cursor::TextInput:
            win32_cursor = IDC_IBEAM;
            break;
        case Cursor::ResizeAll:
            win32_cursor = IDC_SIZEALL;
            break;
        case Cursor::ResizeEW:
            win32_cursor = IDC_SIZEWE;
            break;
        case Cursor::ResizeNS:
            win32_cursor = IDC_SIZENS;
            break;
        case Cursor::ResizeNESW:
            win32_cursor = IDC_SIZENESW;
            break;
        case Cursor::ResizeNWSE:
            win32_cursor = IDC_SIZENWSE;
            break;
        case Cursor::Hand:
            win32_cursor = IDC_HAND;
            break;
        case Cursor::NotAllowed:
            win32_cursor = IDC_NO;
            break;
    }
    ::SetCursor(::LoadCursor(NULL, win32_cursor));
}

bool Window::GetMouseCapture() const
{
    return ::GetCapture() == GetHandle();
}

void Window::SetMouseCapture(bool value)
{
    if (value)
    {
        ::SetCapture((HWND)GetHandle());
    }
    else
    {
        ::ReleaseCapture();
    }
}

bool Window::IsVisible() const
{
    return GetWindowState() != WindowState::Minimized && IsWindowVisible((HWND)GetHandle());
}

bool Window::GetFocus() const
{
    return ::GetFocus() == (HWND)GetHandle();
}

void Window::SetFocus(bool value)
{
    if (value)
        ::SetFocus((HWND)GetHandle());
    else
        ::SetFocus(NULL);
}

std::string Window::GetTitle() const
{
    int32_t len = GetWindowTextLength((HWND)GetHandle());
    TCHAR buffer[4096];
    GetWindowText((HWND)GetHandle(), buffer, len + 1);
    buffer[len] = 0;
    return FromNative(buffer);
}

void Window::SetTitle(const std::string& value)
{
    SetWindowText((HWND)GetHandle(), ToNative(value));
}

Rect<float> Window::GetRect() const
{
    RECT r;
    GetWindowRect((HWND)GetHandle(), &r);
    float dpi = GetDpiForWindow((HWND)GetHandle()) / (float)USER_DEFAULT_SCREEN_DPI;
    return {r.left / dpi, r.top / dpi, (r.right - r.left) / dpi, (r.bottom - r.top) / dpi};
}

void Window::SetRect(const Rect<float>& value)
{
    if (GetWindowState() == WindowState::Maximized)
        return;

    float dpi = GetDpiForWindow((HWND)GetHandle()) / (float)USER_DEFAULT_SCREEN_DPI;
    SetWindowPos((HWND)GetHandle(), NULL, (int)(value.X * dpi), (int)(value.Y * dpi), (int)(value.Width * dpi), (int)(value.Height * dpi), SWP_NOACTIVATE | SWP_NOZORDER);
}

Size<float> Window::GetClientSize() const
{
    RECT r;
    ::GetClientRect((HWND)GetHandle(), &r);
    float dpi = GetDpiForWindow((HWND)GetHandle()) / (float)USER_DEFAULT_SCREEN_DPI;
    return {(r.right - r.left) / dpi, (r.bottom - r.top) / dpi};
}

void Window::SetClientSize(const Size<float>& value)
{
    if (GetWindowState() == WindowState::Maximized)
        return;

    float dpi = GetDpiForWindow((HWND)GetHandle()) / (float)USER_DEFAULT_SCREEN_DPI;
    RECT r = {0, 0, (int)(value.Width * dpi), (int)(value.Height * dpi)};
    AdjustWindowRectEx(&r, GetWindowLong((HWND)GetHandle(), GWL_STYLE), FALSE, GetWindowLong((HWND)GetHandle(), GWL_EXSTYLE));
    auto rect = GetRect();
    rect.Width = (r.right - r.left) / dpi;
    rect.Height = (r.bottom - r.top) / dpi;
    SetRect(rect);
}

WindowState Window::GetWindowState() const
{
    if (IsIconic((HWND)GetHandle()))
        return WindowState::Minimized;

    if (IsZoomed((HWND)GetHandle()))
        return WindowState::Maximized;

    return WindowState::Normal;
}

void Window::SetWindowState(WindowState state)
{
    switch (state)
    {
        case WindowState::Normal:
            ShowWindow((HWND)GetHandle(), SW_RESTORE);
            break;
        case WindowState::Minimized:
            ShowWindow((HWND)GetHandle(), SW_MINIMIZE);
            break;
        case WindowState::Maximized:
            ShowWindow((HWND)GetHandle(), SW_MAXIMIZE);
            break;
    }
}

bool Window::GetTopMost() const
{
    return (GetWindowLong((HWND)GetHandle(), GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
}

void Window::SetTopMost(bool value)
{
    SetWindowPos((HWND)GetHandle(), value ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
}

float Window::GetTransparency() const
{
    BYTE alpha;
    DWORD flag = LWA_ALPHA;
    GetLayeredWindowAttributes((HWND)GetHandle(), NULL, &alpha, &flag);
    return std::clamp(alpha / (float)0xFF, 0.f, 1.f);
}

void Window::SetTransparency(float alpha)
{
    SetLayeredWindowAttributes((HWND)GetHandle(), 0, (BYTE)(alpha * 0xFF), LWA_ALPHA);
}

void Window::MoveToCenter()
{
    if (GetWindowState() != WindowState::Normal)
        return;

    auto r = GetRect();

    RECT rectWorkArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, SPIF_SENDCHANGE); // 获取屏幕客户区大小

    float dpi = GetDpiForWindow((HWND)GetHandle()) / (float)USER_DEFAULT_SCREEN_DPI;

    r.X = ((rectWorkArea.right - rectWorkArea.left) / dpi - r.Width) / 2;
    r.Y = ((rectWorkArea.bottom - rectWorkArea.top) / dpi - r.Height) / 2;

    SetRect(r);
}

Window::~Window()
{
    Close();
}
#endif