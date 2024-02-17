#include <condition_variable>
#include <chrono>
#include <thread>
#include "Application.h"
#include "Window.h"

using namespace tk;

Application* app = nullptr;
Window* mainWindow = nullptr;
std::map<Window*, std::function<void()>> windows;

extern bool IsMainThread();
extern void AppInit();
extern void AppUnInit();

bool UpdateAllWindows(Application* app)
{
    if (windows.size() == 0)
        return false;

    if (mainWindow != nullptr && windows.find(mainWindow) == windows.end())
    {
        app->Exit();
        return false;
    }

    auto wins = windows;

    for (auto&& it : wins)
    {
        it.second();
    }

    return true;
}
void RunLoop(Application* app, Window* win)
{
    while (true)
    {
        auto begin = std::chrono::high_resolution_clock::now();
        if (!app->Update())
            break;

        if (win != nullptr && windows.find(win) == windows.end())
            break;

        if (!UpdateAllWindows(app))
            break;

        auto end = std::chrono::high_resolution_clock::now();

        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        if (dt.count() < 33)
            std::this_thread::sleep_for(std::chrono::milliseconds(33) - dt);
    }
}
void RegisterWindow(Window* win, const std::function<void()>& updater)
{
    windows.emplace(win, updater);
}
void UnRegisterWindow(Window* win)
{
    auto it = windows.find(win);
    if (it != windows.end())
        windows.erase(it);
}

Application* Application::Current()
{
    return app;
}

Application::Application()
{
    app = this;
    AppInit();
}

Application::~Application()
{
    app = nullptr;
}

Window* Application::GetMainWindow()
{
    return mainWindow;
}

int32_t Application::Run(Window* win)
{
    mainWindow = win;

    if (mainWindow != nullptr && !mainWindow->IsVisible())
        mainWindow->Show();

    RunLoop(this, nullptr);
    return 0;
}