#pragma once
#include <stdint.h>
#include <functional>

namespace tk
{
class Window;

class Application
{
public:
    Application();

    void InvokeAsync(const std::function<void()>& f);

    void Invoke(const std::function<void()>& f);

    template <typename R>
    R Invoke(const std::function<R()>& f)
    {
        R ret;
        Invoke([&]()
            { ret = f(); });
        return ret;
    }

    //#ifdef _WIN32
    //tk::StreamPtr LoadResource(int32_t id);
    //#else
    //tk::StreamPtr LoadResource(const std::string& name);
    //#endif

    Window* GetMainWindow();

    bool Update();

    int32_t Run(Window* win);

    template <typename WINDOW>
    int32_t Run()
    {
        WINDOW win;
        win.Create();
        return Run(&win);
    }

    void Exit();
};
} // namespace tk
