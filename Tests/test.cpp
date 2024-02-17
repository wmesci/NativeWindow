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
    //win.AddStyle(WINDOW_NOTITLE);
    win.RemoveStyle(WINDOW_RESIZABLE);
    win.RemoveStyle(WINDOW_BUTTON_MAX);
    win.Create();
    win.SetTopMost(true);
    win.MoveToCenter();
    app.Run(&win);

    return 0;
}