## NativeWindow
> Create and manipulate Native Window in a unified way on both Windows and Mac systems.

[![CMake on multiple platforms](https://github.com/wmesci/NativeWindow/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/wmesci/NativeWindow/actions/workflows/cmake-multi-platform.yml)

```cpp
#include "Window.h"
#include "Application.h"

using namespace tk;

class TestWindow : public Window
{
public:
    virtual bool Create() override
    {
        return CreateImpl(nullptr, "Title", {100, 100, 960, 640});
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    Application app;

    TestWindow win;
    win.Create();
    win.SetTopMost(true);
    win.MoveToCenter();
    app.Run(&win);

    return 0;
}
```
