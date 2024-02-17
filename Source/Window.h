#pragma once
#ifdef _WIN32
#include <Windows.h>
#endif

#include <string>
#include <functional>
#include <map>
#include <stdint.h>

namespace tk
{
template <typename T>
struct Point
{
    T X;
    T Y;

    bool operator==(const Point<T>& other) const
    {
        return X == other.X && Y == other.Y;
    }

    bool operator!=(const Point<T>& other) const
    {
        return X != other.X || Y != other.Y;
    }
};

template <typename T>
struct Size
{
    T Width;
    T Height;

    bool operator==(const Size<T>& other) const
    {
        return Width == other.Width && Height == other.Height;
    }

    bool operator!=(const Size<T>& other) const
    {
        return Width != other.Width || Height != other.Height;
    }
};

#if _MSC_VER
#pragma warning(disable : 4201) // 使用了非标准扩展: 无名称的结构/联合
#endif

template <typename T>
struct Rect
{
    Rect() = default;

    Rect(T x, T y, T width, T height)
        : X(x)
        , Y(y)
        , Width(width)
        , Height(height)
    {
    }

    Rect(const Point<T>& pos, const Size<T>& size)
        : Position(pos)
        , Size(size)
    {
    }

    bool operator==(const Rect<T>& other) const
    {
        return Position == other.Position && Size == other.Size;
    }

    bool operator!=(const Rect<T>& other) const
    {
        return Position != other.Position || Size != other.Size;
    }

    union
    {
        struct
        {
            T X;
            T Y;
        };
        Point<T> Position;
    };

    union
    {
        struct
        {
            T Width;
            T Height;
        };
        Size<T> Size;
    };
};

#if _MSC_VER
#pragma warning(default : 4201) // 使用了非标准扩展: 无名称的结构/联合
#endif

struct NativeWindow;

enum class EventType
{
    None,
    Create,
    Closing,
    Closed,
    Resize,
    DpiChanged,
    VisibleChanged,

    Input,

    KeyDown,
    KeyUp,
    KeyPress,

    MouseEnter,
    MouseExit,
    MouseDown,
    MouseUp,
    MouseMove,
    MouseWheel,
    MouseClick,
    MouseDoubleClick
};

struct Event
{
    EventType type = EventType::None;
    uint32_t result = 0;
};

enum class MouseButton
{
    Left,
    Middle,
    Right
};

struct MouseWheelEvent : public Event
{
    float WheelX;
    float WheelY;
};

struct MouseButtonEvent : public Event
{
    MouseButton Button;
};

enum class ModifierKey
{
    None = 0,
    LeftAlt = 0x01,
    RightAlt = 0x02,
    LeftCtrl = 0x04,
    RightCtrl = 0x08,
    LeftShift = 0x10,
    RightShift = 0x20,
    LeftMeta = 0x40,
    RightMeta = 0x80,
};

enum class Keys
{
    None = 0,
    Esc,
    Return,
    Tab,
    Space,
    Backspace,
    Up,
    Down,
    Left,
    Right,
    Insert,
    Delete,
    Home,
    End,
    PageUp,
    PageDown,
    Print,
    Plus,
    Minus,
    LeftBracket,
    RightBracket,
    Semicolon,
    Quote,
    Comma,
    Period,
    Slash,
    Backslash,
    Tilde,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    NumPad0,
    NumPad1,
    NumPad2,
    NumPad3,
    NumPad4,
    NumPad5,
    NumPad6,
    NumPad7,
    NumPad8,
    NumPad9,
    Key0,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
    KeyA,
    KeyB,
    KeyC,
    KeyD,
    KeyE,
    KeyF,
    KeyG,
    KeyH,
    KeyI,
    KeyJ,
    KeyK,
    KeyL,
    KeyM,
    KeyN,
    KeyO,
    KeyP,
    KeyQ,
    KeyR,
    KeyS,
    KeyT,
    KeyU,
    KeyV,
    KeyW,
    KeyX,
    KeyY,
    KeyZ,
    GamepadA,
    GamepadB,
    GamepadX,
    GamepadY,
    GamepadThumbL,
    GamepadThumbR,
    GamepadShoulderL,
    GamepadShoulderR,
    GamepadUp,
    GamepadDown,
    GamepadLeft,
    GamepadRight,
    GamepadBack,
    GamepadStart,
    GamepadGuide,
};

enum class Cursor
{
    None = -1,
    Arrow = 0,
    TextInput,
    ResizeAll,
    ResizeNS,
    ResizeEW,
    ResizeNESW,
    ResizeNWSE,
    Hand,
    NotAllowed,
};

struct KeyEvent : public Event
{
    ModifierKey Modifier;
    Keys Key;
};

struct InputEvent : public Event
{
    uint32_t Char;
};

enum class WindowState
{
    Normal,
    Minimized,
    Maximized
};

constexpr int32_t WINDOW_NOTITLE = 1 << 0;
constexpr int32_t WINDOW_BUTTON_MIN = 1 << 1;
constexpr int32_t WINDOW_BUTTON_MAX = 1 << 2;
constexpr int32_t WINDOW_BUTTON_CLOSE = 1 << 3;
constexpr int32_t WINDOW_RESIZABLE = 1 << 4;

class Window
{
    friend void DispatchEvent(Window* win, Event* e);

#ifdef _WIN32
    friend LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

protected:
    bool CreateImpl(Window* parent, std::string title, const Rect<float>& rect);

#ifdef _WIN32
    virtual bool WndProc([[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam, [[maybe_unused]] LRESULT* result)
    {
        return false;
    }
#endif

    virtual void OnEvent(Event* e);

    virtual void OnCreate() {}

    virtual void OnUpdate() {}

    virtual bool OnClosing() { return true; }

    virtual void OnClose() {}

    virtual void OnVisibleChanged() {}

    virtual void OnStyleChanged();

public:
    Window();

    virtual bool Create() = 0;

    uint32_t AddEventListener(const std::function<void(Window*, Event*)>& callback);
    bool RemoveEventListener(uint32_t id);

    void Show();

    void Hide();

    void Close();

    void ShowDialog();

    void* GetHandle() const;

    NativeWindow* GetNativeWindow() const { return nativeWindow; }

    float GetDpiScale() const;

    Point<float> GetMousePosition() const;
    void SetMousePosition(const Point<float>& p);

    // Cursor GetCursor() const;
    void SetCursor(const Cursor& cur);

    bool GetMouseCapture() const;
    void SetMouseCapture(bool value);

    bool IsVisible() const;

    bool GetFocus() const;
    void SetFocus(bool);

    std::string GetTitle() const;
    void SetTitle(const std::string&);

    Rect<float> GetRect() const;
    void SetRect(const Rect<float>&);

    Size<float> GetClientSize() const;
    void SetClientSize(const Size<float>&);

    WindowState GetWindowState() const;
    void SetWindowState(WindowState);

    bool GetTopMost() const;
    void SetTopMost(bool);

    float GetTransparency() const;
    void SetTransparency(float alpha);

    int32_t GetStyle() const { return style; }
    void SetStyle(int32_t value)
    {
        if (style != value)
        {
            style = value;
            OnStyleChanged();
        }
    }

    bool HasStyle(int32_t value) { return (style & value) != 0; }

    void AddStyle(int32_t value) { SetStyle(style | value); }

    void RemoveStyle(int32_t value) { SetStyle(style & ~value); }

    void MoveToCenter();

    virtual ~Window();

private:
    NativeWindow* nativeWindow = nullptr;
    int32_t style = WINDOW_RESIZABLE | WINDOW_BUTTON_MIN | WINDOW_BUTTON_MAX | WINDOW_BUTTON_CLOSE;
    uint32_t event_id = 0;
    std::map<uint32_t, std::function<void(Window*, Event*)>> listeners;
};
} // namespace tk
