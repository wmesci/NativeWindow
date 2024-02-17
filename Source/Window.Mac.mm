#if defined(__APPLE__)
#include "TargetConditionals.h"
#if defined(TARGET_OS_MAC)

#import <Cocoa/Cocoa.h>
#include "Application.h"
#include "Window.h"

using namespace tk;

uint32_t ON_UPDATE = 1;
uint32_t ON_CLOSING = 2;

namespace tk
{
struct NativeWindow
{
    NSWindow* window;
    NSObject* delegate;

    ~NativeWindow()
    {
        if (window)
        {
            [window close];
            window = nil;
        }
        delegate = nil;
    }
};
} // namespace tk

bool DispatchEvent(NSWindow* nswin, NSEvent* event);

@interface NativeView : NSView

@end

@implementation NativeView

- (BOOL)acceptsFirstResponder
{
    return (YES);
}

- (BOOL)becomeFirstResponder
{
    return (YES);
}

- (BOOL)resignFirstResponder
{
    return (YES);
}

//- (BOOL)acceptsFirstMouse:(NSEvent*)event
//{
//    return YES;
//}

- (void)keyUp:(NSEvent*)event
{
    DispatchEvent([self window], event);
}
- (void)keyDown:(NSEvent*)event
{
    DispatchEvent([self window], event);
}
- (void)flagsChanged:(NSEvent*)event
{
    DispatchEvent([self window], event);
}
- (void)mouseEntered:(NSEvent*)event
{
    DispatchEvent([self window], event);
}
- (void)mouseExited:(NSEvent*)event
{
    DispatchEvent([self window], event);
}
- (void)mouseDown:(NSEvent*)event
{
    DispatchEvent([self window], event);
    [super mouseDown:event];
}
- (void)mouseUp:(NSEvent*)event
{
    DispatchEvent([self window], event);
    [super mouseUp:event];
}
- (void)mouseMoved:(NSEvent*)event
{
    DispatchEvent([self window], event);
    [super mouseMoved:event];
}
- (void)scrollWheel:(NSEvent*)event
{
    DispatchEvent([self window], event);
}
- (void)rightMouseDown:(NSEvent*)event
{
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
    DispatchEvent([self window], event);
}
- (void)rightMouseUp:(NSEvent*)event
{
    DispatchEvent([self window], event);
}
- (void)mouseDragged:(NSEvent*)event
{
    DispatchEvent([self window], event);
}

- (void)magnifyWithEvent:(NSEvent*)event
{
    DispatchEvent([self window], event);
}

- (void)rotateWithEvent:(NSEvent*)event
{
    DispatchEvent([self window], event);
}

- (void)swipeWithEvent:(NSEvent*)event
{
    DispatchEvent([self window], event);
}
@end

// 使用NSWindowDelegate来处理左上角关闭按钮的逻辑
@interface NativeWindowDelegate
    : NSObject <NSWindowDelegate, NSDraggingDestination>
- (BOOL)windowShouldClose:(NSWindow*)sender;
- (void)windowWillClose:(NSNotification*)notification;
- (void)windowDidResize:(NSWindow*)sender;
- (void)windowDidChangeBackingProperties:(NSNotification*)notification;
- (void)setWindow:(Window*)win;
- (Window*)getWindow;
@property(nonatomic) Window* window;
@end

@implementation NativeWindowDelegate

- (void)setWindow:(Window*)win
{
    _window = win;
}

- (Window*)getWindow
{
    return _window;
}

- (BOOL)windowShouldClose:(NSWindow*)sender
{
    Event e;
    e.type = EventType::Closing;
    DispatchEvent(_window, &e);
    return e.result == 0;
}

- (void)windowWillClose:(NSNotification*)notification
{
    Event e;
    e.type = EventType::Closed;
    DispatchEvent(_window, &e);
}

- (void)windowDidResize:(NSWindow*)sender
{
    Event e;
    e.type = EventType::Resize;
    DispatchEvent(_window, &e);
}
- (void)windowDidChangeBackingProperties:(NSNotification*)notification
{
    Event e;
    e.type = EventType::DpiChanged;
    DispatchEvent(_window, &e);
}
@end

ModifierKey translateModifiers(int flags)
{
    // clang-format off
    int ret = 0
	    | ((0 != (flags & NX_DEVICELSHIFTKEYMASK ) ) ? (int)ModifierKey::LeftShift  : 0)
	    | ((0 != (flags & NX_DEVICERSHIFTKEYMASK ) ) ? (int)ModifierKey::RightShift : 0)
	    | ((0 != (flags & NX_DEVICELALTKEYMASK ) )   ? (int)ModifierKey::LeftAlt    : 0)
	    | ((0 != (flags & NX_DEVICERALTKEYMASK ) )   ? (int)ModifierKey::RightAlt   : 0)
	    | ((0 != (flags & NX_DEVICELCTLKEYMASK ) )   ? (int)ModifierKey::LeftCtrl   : 0)
	    | ((0 != (flags & NX_DEVICERCTLKEYMASK ) )   ? (int)ModifierKey::RightCtrl  : 0)
	    | ((0 != (flags & NX_DEVICELCMDKEYMASK) )    ? (int)ModifierKey::LeftMeta   : 0)
	    | ((0 != (flags & NX_DEVICERCMDKEYMASK) )    ? (int)ModifierKey::RightMeta  : 0)
	    ;
    return (ModifierKey)ret;
    // clang-format on
}

Keys translateKey(uint8_t keyCode)
{
    if (keyCode >= 'A' && keyCode <= 'Z')
    {
        return (Keys)((int)Keys::KeyA + (keyCode - 'A'));
    }

    if (keyCode >= 'a' && keyCode <= 'z')
    {
        return (Keys)((int)Keys::KeyA + (keyCode - 'a'));
    }

    if (keyCode >= '0' && keyCode <= '9')
    {
        return (Keys)((int)Keys::Key0 + (keyCode - '0'));
    }

    switch (keyCode)
    {
        case 27:
            return Keys::Esc;
        case uint8_t('\r'):
            return Keys::Return;
        case uint8_t('\t'):
            return Keys::Tab;
        case 127:
            return Keys::Backspace;
        case uint8_t(' '):
            return Keys::Space;
        case uint8_t('+'):
        case uint8_t('='):
            return Keys::Plus;
        case uint8_t('_'):
        case uint8_t('-'):
            return Keys::Minus;
        case uint8_t('~'):
        case uint8_t('`'):
            return Keys::Tilde;
        case uint8_t(':'):
        case uint8_t(';'):
            return Keys::Semicolon;
        case uint8_t('"'):
        case uint8_t('\''):
            return Keys::Quote;
        case uint8_t('{'):
        case uint8_t('['):
            return Keys::LeftBracket;
        case uint8_t('}'):
        case uint8_t(']'):
            return Keys::RightBracket;
        case uint8_t('<'):
        case uint8_t(','):
            return Keys::Comma;
        case uint8_t('>'):
        case uint8_t('.'):
            return Keys::Period;
        case uint8_t('?'):
        case uint8_t('/'):
            return Keys::Slash;
        case uint8_t('|'):
        case uint8_t('\\'):
            return Keys::Backslash;
    }

    return Keys::None;
}

Keys handleKeyEvent(NSEvent* event, ModifierKey* specialKeys)
{
    NSString* key = [event charactersIgnoringModifiers];
    if ([key length] == 0)
    {
        return Keys::None;
    }

    *specialKeys = translateModifiers(int([event modifierFlags]));

    int keyCode = [key characterAtIndex:0];

    if (keyCode < 256)
    {
        return translateKey(keyCode);
    }

    switch (keyCode)
    {
        case NSF1FunctionKey:
            return Keys::F1;
        case NSF2FunctionKey:
            return Keys::F2;
        case NSF3FunctionKey:
            return Keys::F3;
        case NSF4FunctionKey:
            return Keys::F4;
        case NSF5FunctionKey:
            return Keys::F5;
        case NSF6FunctionKey:
            return Keys::F6;
        case NSF7FunctionKey:
            return Keys::F7;
        case NSF8FunctionKey:
            return Keys::F8;
        case NSF9FunctionKey:
            return Keys::F9;
        case NSF10FunctionKey:
            return Keys::F10;
        case NSF11FunctionKey:
            return Keys::F11;
        case NSF12FunctionKey:
            return Keys::F12;

        case NSLeftArrowFunctionKey:
            return Keys::Left;
        case NSRightArrowFunctionKey:
            return Keys::Right;
        case NSUpArrowFunctionKey:
            return Keys::Up;
        case NSDownArrowFunctionKey:
            return Keys::Down;

        case NSPageUpFunctionKey:
            return Keys::PageUp;
        case NSPageDownFunctionKey:
            return Keys::PageDown;
        case NSHomeFunctionKey:
            return Keys::Home;
        case NSEndFunctionKey:
            return Keys::End;

        case NSPrintScreenFunctionKey:
            return Keys::Print;
    }

    return Keys::None;
}

void tk::DispatchEvent(Window* win, Event* e)
{
    win->OnEvent(e);

    if (e->type == EventType::Closed && win->nativeWindow)
    {
        win->nativeWindow->window = nil;
        win->nativeWindow->delegate = nil;
    }
}

bool DispatchEvent(NSWindow* nswin, NSEvent* event)
{
    NativeWindowDelegate* delegate = (NativeWindowDelegate*)[nswin delegate];
    Window* window = [delegate getWindow];

    NSEventType eventType = [event type];

    switch (eventType)
    {
        case NSEventTypeMouseEntered:
        {
            Event e;
            e.type = EventType::MouseEnter;
            DispatchEvent(window, &e);
        }
        break;
        case NSEventTypeMouseExited:
        {
            Event e;
            e.type = EventType::MouseExit;
            DispatchEvent(window, &e);
        }
        break;
        case NSEventTypeLeftMouseDown:
        {
            MouseButtonEvent e;
            e.type = EventType::MouseDown;
            e.Button = MouseButton::Left;
            DispatchEvent(window, &e);
        }
        break;
        case NSEventTypeLeftMouseUp:
        {
            MouseButtonEvent e;
            e.type = EventType::MouseUp;
            e.Button = MouseButton::Left;
            DispatchEvent(window, &e);
        }
        break;
        case NSEventTypeRightMouseDown:
        {
            MouseButtonEvent e;
            e.type = EventType::MouseDown;
            e.Button = MouseButton::Right;
            DispatchEvent(window, &e);
        }
        break;
        case NSEventTypeRightMouseUp:
        {
            MouseButtonEvent e;
            e.type = EventType::MouseUp;
            e.Button = MouseButton::Right;
            DispatchEvent(window, &e);
        }
        break;
        case NSEventTypeOtherMouseDown:
        {
            MouseButtonEvent e;
            e.type = EventType::MouseDown;
            e.Button = MouseButton::Middle;
            DispatchEvent(window, &e);
        }
        break;
        case NSEventTypeOtherMouseUp:
        {
            MouseButtonEvent e;
            e.type = EventType::MouseUp;
            e.Button = MouseButton::Middle;
            DispatchEvent(window, &e);
        }
        break;
        case NSEventTypeMouseMoved:
        case NSEventTypeLeftMouseDragged:
        case NSEventTypeRightMouseDragged:
        case NSEventTypeOtherMouseDragged:
        {
            Event e;
            e.type = EventType::MouseMove;
            DispatchEvent(window, &e);
        }
        break;
        case NSEventTypeScrollWheel:
        {
            MouseWheelEvent e;
            e.type = EventType::MouseWheel;
            e.WheelX = [event deltaX];
            e.WheelY = [event deltaY];
            DispatchEvent(window, &e);
        }
        break;
        case NSEventTypeKeyDown:
        {
            ModifierKey modifiers = ModifierKey::None;
            Keys key = handleKeyEvent(event, &modifiers);

            if (key != Keys::None)
            {
                bool ctrl = ((int)modifiers & (int)ModifierKey::LeftCtrl) != 0 || ((int)modifiers & (int)ModifierKey::RightCtrl) != 0;
                NSString* str = [event characters];
                int len = (int)[str length];
                for (int i = 0; i < len; i++)
                {
                    int c = [str characterAtIndex:i];
                    if (!ctrl && !(c >= 0xF700 && c <= 0xFFFF) && c != 127)
                    {
                        InputEvent e;
                        e.type = EventType::Input;
                        e.Char = (unsigned int)c;
                        DispatchEvent(window, &e);
                    }
                }

                if ([event isARepeat])
                {
                    KeyEvent e;
                    e.type = EventType::KeyPress;
                    e.Modifier = modifiers;
                    e.Key = key;
                    DispatchEvent(window, &e);
                }
                else
                {
                    KeyEvent e;
                    e.type = EventType::KeyDown;
                    e.Modifier = modifiers;
                    e.Key = key;
                    DispatchEvent(window, &e);
                }
                return true;
            }
        }
        break;

        case NSEventTypeKeyUp:
        {
            ModifierKey modifiers = ModifierKey::None;
            Keys key = handleKeyEvent(event, &modifiers);

            if (key != Keys::None)
            {
                KeyEvent e;
                e.type = EventType::KeyUp;
                e.Modifier = modifiers;
                e.Key = key;
                DispatchEvent(window, &e);

                return true;
            }
        }
        break;
        case NSEventTypeFlagsChanged:
        {
        }
        break;
        default:
            break;
    }

    return false;
}

void Window::OnStyleChanged()
{
    if (nativeWindow != NULL)
    {
        id window = (id)GetHandle();
        NSUInteger winStyle = [window styleMask];
        if (style & WINDOW_NOTITLE)
            winStyle &= ~NSWindowStyleMaskTitled;
        else
            winStyle |= NSWindowStyleMaskTitled;

        if (style & WINDOW_BUTTON_MIN)
            winStyle |= NSWindowStyleMaskMiniaturizable;
        else
            winStyle &= ~NSWindowStyleMaskMiniaturizable;

        if (style & WINDOW_BUTTON_MAX)
            [[window standardWindowButton:NSWindowZoomButton] setEnabled:YES];
        else
            [[window standardWindowButton:NSWindowZoomButton] setEnabled:NO];

        if (style & WINDOW_RESIZABLE)
            winStyle |= NSWindowStyleMaskResizable;
        else
            winStyle &= ~NSWindowStyleMaskResizable;

        [window setStyleMask:winStyle];
    }
}

bool Window::CreateImpl(Window* parent, std::string title, const Rect<float>& rect)
{
    @autoreleasepool
    {
        NSView* view = [[NativeView alloc] initWithFrame:NSMakeRect(0, 0, rect.Width, rect.Height)];

        NSUInteger style = NSWindowStyleMaskTitled |
                           NSWindowStyleMaskMiniaturizable |
                           NSWindowStyleMaskResizable | NSWindowStyleMaskClosable;
        id window = [[NSWindow alloc]
            initWithContentRect:NSMakeRect(rect.X, rect.Y, rect.Width, rect.Height)
                      styleMask:style
                        backing:NSBackingStoreBuffered
                          defer:NO];
        [window setTitle:[NSString stringWithUTF8String:title.c_str()]];
        [window setContentView:view];
        [window makeFirstResponder:view];
        [window makeKeyAndOrderFront:nil];
        [window setAcceptsMouseMovedEvents:YES];
        [NSWindow setAllowsAutomaticWindowTabbing:NO];

        NSTrackingAreaOptions options = (NSTrackingActiveAlways | NSTrackingInVisibleRect | NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved);
        NSTrackingArea* trackingArea = [[NSTrackingArea alloc] initWithRect:[view bounds] options:options owner:view userInfo:nil];
        [view addTrackingArea:trackingArea];

        id _windowDelegate = [[NativeWindowDelegate alloc] init];
        [_windowDelegate setWindow:this];
        [window setDelegate:_windowDelegate];
        if (parent != nullptr && parent->GetHandle() != nullptr)
        {
            id _parent = (id)parent->GetHandle();
            [_parent addChildWindow:window ordered:NSWindowAbove];
        }

        nativeWindow = new NativeWindow();
        nativeWindow->window = window;
        nativeWindow->delegate = _windowDelegate;

        Event e;
        e.type = EventType::Create;
        e.result = 0;
        DispatchEvent(this, &e);

        this->OnCreate();
    }

    SetTransparency(1);

    OnStyleChanged();

    return nativeWindow != NULL;
}

void Window::Show()
{
    id window = (id)GetHandle();
    [window setIsVisible:YES];
}

void Window::Hide()
{
    id window = (id)GetHandle();
    [window setIsVisible:NO];
}

void Window::Close()
{
    id window = (id)GetHandle();
    [window close];
}

void Window::ShowDialog() {}

void* Window::GetHandle() const
{
    if (nativeWindow == nullptr)
        return nil;

    return nativeWindow->window;
}

float Window::GetDpiScale() const
{
    id window = (id)GetHandle();
    return [window backingScaleFactor];
}

tk::Point<float> Window::GetMousePosition() const
{
    NSWindow* window = (NSWindow*)GetHandle();
    NSRect originalFrame = [window frame];
    NSPoint location = [window mouseLocationOutsideOfEventStream];
    NSRect adjustFrame = [window contentRectForFrameRect:originalFrame];
    int32_t x = location.x;
    int32_t y = adjustFrame.size.height - location.y;
    return {(float)std::clamp((float)x, 0.f, (float)adjustFrame.size.width), (float)std::clamp((float)y, 0.0f, (float)adjustFrame.size.height)};
}

void Window::SetMousePosition(const tk::Point<float>& p)
{
    // TODO:
}

void Window::SetCursor(const Cursor& cur)
{
    NSCursor* _cursor = [NSCursor arrowCursor];
    switch (cur)
    {
        case Cursor::None:
            _cursor = nil;
            break;
        case Cursor::Arrow:
            _cursor = [NSCursor arrowCursor];
            break;
        case Cursor::TextInput:
            _cursor = [NSCursor IBeamCursor];
            break;
        case Cursor::ResizeAll:
            _cursor = [NSCursor closedHandCursor];
            break;
        case Cursor::ResizeEW:
            _cursor = [NSCursor resizeLeftRightCursor];
            break;
        case Cursor::ResizeNS:
            _cursor = [NSCursor resizeUpDownCursor];
            break;
        case Cursor::ResizeNESW:
            _cursor = [NSCursor closedHandCursor];
            break;
        case Cursor::ResizeNWSE:
            _cursor = [NSCursor closedHandCursor];
            break;
        case Cursor::Hand:
            _cursor = [NSCursor pointingHandCursor];
            break;
        case Cursor::NotAllowed:
            _cursor = [NSCursor operationNotAllowedCursor];
            break;
    }

    if (_cursor != NSCursor.currentCursor)
    {
        if (_cursor != nil)
        {
            [_cursor set];
            [NSCursor unhide];
        }
        else
        {
            [NSCursor hide];
        }
    }
}

bool Window::GetMouseCapture() const
{
    // TODO:
    return false;
}

void Window::SetMouseCapture(bool value)
{
    // TODO:
}

bool Window::IsVisible() const
{
    id window = (id)GetHandle();
    return GetWindowState() != WindowState::Minimized && [window isVisible];
}

bool Window::GetFocus() const
{
    return false;
}

void Window::SetFocus(bool value)
{
    id window = (id)GetHandle();
    [window makeKeyAndOrderFront:nil];
}

std::string Window::GetTitle() const
{
    id window = (id)GetHandle();
    return std::string([[window title] UTF8String]);
}

void Window::SetTitle(const std::string& value)
{
    id window = (id)GetHandle();
    [window setTitle:[NSString stringWithUTF8String:value.c_str()]];
}

tk::Rect<float> Window::GetRect() const
{
    id window = (id)GetHandle();
    NSRect rect = [window frame];
    NSRect screenRect = [[NSScreen mainScreen] visibleFrame];
    return {(float)rect.origin.x, (float)(screenRect.size.height - rect.origin.y), (float)rect.size.width, (float)rect.size.height};
}

void Window::SetRect(const tk::Rect<float>& value)
{
    // if (GetWindowState() == WindowState::Maximized)
    //     return;

    id window = (id)GetHandle();
    NSRect screenRect = [[NSScreen mainScreen] visibleFrame];
    [window setFrame:NSMakeRect(value.X, screenRect.size.height - value.Y, value.Width, value.Height) display:NO];
}

tk::Size<float> Window::GetClientSize() const
{
    id window = (id)GetHandle();
    id view = [window contentView];
    return {(float)[view bounds].size.width, (float)[view bounds].size.height};
}

void Window::SetClientSize(const tk::Size<float>& value)
{
    // if (GetWindowState() == WindowState::Maximized)
    //     return;

    id window = (id)GetHandle();
    NSRect cur_rect = [window frame];
    NSRect dst_rect = [window frameRectForContentRect:NSMakeRect(0, 0, value.Width, value.Height)];
    dst_rect.origin = cur_rect.origin;

    NSRect screenRect = [[NSScreen mainScreen] visibleFrame];
    SetRect({(float)dst_rect.origin.x, (float)(screenRect.size.height - dst_rect.origin.y), (float)dst_rect.size.width, (float)dst_rect.size.height});
}

WindowState Window::GetWindowState() const
{
    id window = (id)GetHandle();

    if ([window isZoomed])
        return WindowState::Maximized;

    if ([window isMiniaturized])
        return WindowState::Minimized;

    return WindowState::Normal;
}

void Window::SetWindowState(WindowState state)
{
    id window = (id)GetHandle();

    switch (state)
    {
        case WindowState::Normal:
            if ([window isMiniaturized])
                [window setIsMiniaturized:NO];
            else if ([window isZoomed])
                [window setIsZoomed:NO];
            break;
        case WindowState::Minimized:
            [window miniaturize:nil];
            break;
        case WindowState::Maximized:
            [window zoom:nil];
            break;
    }
}

bool Window::GetTopMost() const
{
    id window = (id)GetHandle();
    return [window level] == NSMainMenuWindowLevel;
}

void Window::SetTopMost(bool value)
{
    id window = (id)GetHandle();
    if (value)
        [window setLevel:NSMainMenuWindowLevel];
    else
        [window setLevel:NSNormalWindowLevel];
}

float Window::GetTransparency() const
{
    id window = (id)GetHandle();
    return [window alphaValue];
}

void Window::SetTransparency(float alpha)
{
    id window = (id)GetHandle();
    [window setAlphaValue:alpha];
}

void Window::MoveToCenter()
{
    if (GetWindowState() != WindowState::Normal)
        return;

    NSWindow* window = (id)GetHandle();
    [window center];
}

Window::~Window()
{
    if (nativeWindow != nullptr)
    {
        delete nativeWindow;
        nativeWindow = nullptr;
    }
}
#endif
#endif