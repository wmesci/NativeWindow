#ifdef _WIN32
#include <ShellScalingApi.h>
#include <condition_variable>
#include "Application.h"
#include "Window.h"

using namespace tk;

DWORD mainThread;

static const UINT WM_INVOKE = WM_USER + 0x121;

bool IsMainThread()
{
    return GetCurrentThreadId() == mainThread;
}

void AppInit()
{
    mainThread = GetCurrentThreadId();

    // EnableHiDPI
    do
    {
        auto user32 = LoadLibrary(TEXT("user32.dll"));
        if (user32 != NULL)
        {
            auto proc = GetProcAddress(user32, "SetProcessDpiAwarenessContext");
            if (proc != nullptr)
            {
                ((std::add_pointer_t<decltype(SetProcessDpiAwarenessContext)>)proc)(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); // Windows 10，版本1607以上
            }
            FreeLibrary(user32);
            break;
        }

        auto shcore = LoadLibrary(TEXT("shcore.dll"));
        if (shcore != nullptr)
        {
            auto proc = GetProcAddress(shcore, "SetProcessDpiAwareness");
            if (proc != nullptr)
            {
                ((std::add_pointer_t<decltype(SetProcessDpiAwareness)>)proc)(PROCESS_PER_MONITOR_DPI_AWARE); // Windows 8.1以上
            }
            FreeLibrary(shcore);
            break;
        }

        SetProcessDPIAware(); // Windows Vista
    } while (false);
}

void Application::InvokeAsync(const std::function<void()>& f)
{
    PostThreadMessage(mainThread, WM_INVOKE, (WPARAM) new std::function<void()>(f), TRUE);
}

void Application::Invoke(const std::function<void()>& f)
{
    if (IsMainThread())
    {
        f();
    }
    else
    {
        std::mutex g_lockqueue;
        std::unique_lock<std::mutex> locker(g_lockqueue);
        std::condition_variable g_queuecheck;
        InvokeAsync([&]()
                    {
            f();
            g_queuecheck.notify_one(); });
        g_queuecheck.wait(locker);
    }
}

//rtti::StreamPtr Application::LoadResource(int32_t id)
//{
//    auto handle = FindResource(NULL, MAKEINTRESOURCE(id), RT_RCDATA);
//    if (handle != NULL)
//    {
//        auto hres = LoadResource(NULL, handle);
//        if (hres != nullptr)
//        {
//            return std::make_shared<MemoryStream>(LockResource(hres), SizeofResource(NULL, handle));
//        }
//    }
//    return nullptr;
//}

bool Application::Update()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
        if (msg.message == WM_INVOKE)
        {
            auto f = (std::function<void()>*)msg.wParam;
            if (f != nullptr)
            {
                (*f)();
                
                delete f;
                f = nullptr;
            }
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.message != WM_QUIT;
}

void Application::Exit()
{
    PostQuitMessage(0);
}

#endif
