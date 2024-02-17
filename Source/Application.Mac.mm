#if defined(__APPLE__)
#include "TargetConditionals.h"
#if defined(TARGET_OS_MAC)

#import <Cocoa/Cocoa.h>
#include <filesystem>
#include <condition_variable>
#include <pthread.h>
#include "Application.h"
#include "Window.h"

using namespace tk;

@interface MainAppDelegate : NSObject <NSApplicationDelegate>
{
}
@end

@implementation MainAppDelegate

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)theApplication
{
    return YES;
}

- (void)setupMenu
{
    NSMenu* mainMenuBar = [[NSMenu alloc] init];
    NSMenu* appMenu;
    NSMenuItem* menuItem;

    appMenu = [[NSMenu alloc] initWithTitle:@"MainMenu"];
    menuItem = [appMenu addItemWithTitle:@"退出" action:@selector(terminate:) keyEquivalent:@"q"];
    [menuItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];

    menuItem = [[NSMenuItem alloc] init];
    [menuItem setSubmenu:appMenu];

    [mainMenuBar addItem:menuItem];

    appMenu = nil;
    [NSApp setMainMenu:mainMenuBar];
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
    // Make the application a foreground application (else it won't receive keyboard events)
    ProcessSerialNumber psn = {0, kCurrentProcess};
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);

    // Menu
    [self setupMenu];
}
@end

bool isRunning;
pthread_t mainThread;
int32_t exitcode = 0;

bool IsMainThread()
{
    return pthread_self() == mainThread;
}

void AppInit()
{
    mainThread = pthread_self();

    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];
    MainAppDelegate* delegate = [[MainAppDelegate alloc] init];
    [[NSApplication sharedApplication] setDelegate:delegate];
    [NSApp finishLaunching];

    isRunning = true;
}

void Application::InvokeAsync(const std::function<void()>& f)
{
    dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        f();
    });
}

void Application::Invoke(const std::function<void()>& f)
{
    dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        f();
    });
}

//StreamPtr Application::LoadResource(const std::string& name)
//{
//    static std::string resources_root;
//    static std::once_flag flag;
//    std::call_once(flag, [&]()
//                   {
//        CFURLRef dataFileURL = CFBundleCopyResourceURL(CFBundleGetMainBundle(), NULL, NULL, NULL);
//        CFStringRef path;
//        CFURLCopyResourcePropertyForKey(dataFileURL, kCFURLPathKey, &path, NULL);
//        resources_root = std::filesystem::path(CFStringGetCStringPtr(path, kCFStringEncodingUTF8)).parent_path();
//        if (dataFileURL)
//            CFRelease(dataFileURL);
//            
//        if (path)
//            CFRelease(path); });
//
//    if (std::filesystem::exists(resources_root + "/" + name))
//    {
//        return FileStream::Open(resources_root + "/" + name, FileAccess::Read);
//    }
//
//    return nullptr;
//}

bool Application::Update()
{
    @autoreleasepool
    {
        do
        {
            NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                untilDate:nil
                                                   inMode:NSDefaultRunLoopMode
                                                  dequeue:YES];
            if (event)
            {
                [NSApp sendEvent:event];
            }
            else
            {
                break;
            }
        } while (true);
    }

    return isRunning;
}

void Application::Exit()
{
    isRunning = false;
}
#endif
#endif