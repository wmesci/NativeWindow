#include "Window.h"
#include "Application.h"

using namespace tk;

void RegisterWindow(Window* win, const std::function<void()>& updater);
void UnRegisterWindow(Window* win);

Window::Window()
{
    RegisterWindow(this, [this]()
                   { this->OnUpdate(); });
}

void Window::OnEvent(Event* e)
{
    switch (e->type)
    {
        case EventType::Closed:
            UnRegisterWindow(this);
            this->OnClose();
            break;
        case EventType::Closing:
            e->result = this->OnClosing() ? 0 : 1;
            break;
        case EventType::VisibleChanged:
            this->OnVisibleChanged();
            break;
    }

    auto listeners = this->listeners;
    for (auto&& f : listeners)
    {
        f.second(this, e);
    }
}

uint32_t Window::AddEventListener(const std::function<void(Window*, Event*)>& callback)
{
    event_id++;
    this->listeners.emplace(event_id, callback);
    return event_id;
}

bool Window::RemoveEventListener(uint32_t id)
{
    auto it = this->listeners.find(id);
    if (it != this->listeners.end())
    {
        this->listeners.erase(it);
        return true;
    }
    return false;
}