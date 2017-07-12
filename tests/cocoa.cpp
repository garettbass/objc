#include <atomic>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <thread>

#define OBJC_NAMESPACE _app_macos_objc_namespace
#include "../objc.hpp"

//------------------------------------------------------------------------------

// struct retain_count_check {
//     id const obj;
//     long const old_retain_count;
//     const char* file;
//     const long line;
//     const char* name;

//     retain_count_check(id obj, const char* file, long line, const char* name)
//     : obj(obj)
//     , old_retain_count(objc::get_retain_count(obj))
//     , file(file), line(line), name(name) {}

//     ~retain_count_check() {
//         long const new_retain_count { objc::get_retain_count(obj) };
//         if (old_retain_count < new_retain_count) {
//             printf(
//                 "%s:%li '%s' "
//                 "retain count changed %li -> %li\n",
//                 file,line,name,
//                 old_retain_count,
//                 new_retain_count
//             );
//         }
//     }
// };

// #define check_retain_count(id) \
//     retain_count_check check_retain_count_##id {id,__FILE__,__LINE__,#id};

//------------------------------------------------------------------------------

namespace app {

    int main(int argc, char* argv[]);

    bool update(const float timeout_ms = 0);

    void quit();

    template<typename Event>
    struct event {

    };

} // namespace app

//------------------------------------------------------------------------------

namespace app {
namespace macos {

    namespace objc = ::_app_macos_objc_namespace;

    extern "C" {

        extern id const NSDefaultRunLoopMode;

        extern id const NSApplicationDidFinishLaunchingNotification;

        extern id const NSApplicationWillTerminateNotification;

    } // extern "C"

    //--------------------------------------------------------------------------

    class NSApplication {
        id const self {
            objc::message<id()>{"sharedApplication"}(
                objc::class_id("NSApplication")
            )
        };

        objc::message<void(bool)>
        _activateIgnoringOtherApps
        {"activateIgnoringOtherApps:"};

        objc::message<void(id)> _setDelegate {"setDelegate:"};

        objc::message<void()> _finishLaunching {"finishLaunching"};

        objc::message<id(uintptr_t,id,id,bool)>
        _nextEventMatchingMask_untilDate_inMode_dequeue
        {"nextEventMatchingMask:untilDate:inMode:dequeue:"};

        objc::message<void(id)> _sendEvent {"sendEvent:"};

        objc::message<void(id)> _terminate {"terminate:"};

        objc::message<void()> _updateWindows {"updateWindows"};

    public:

        NSApplication() = default;

        operator id() const { return self; }

        void activate() const { _activateIgnoringOtherApps(self,true); }

        void setDelegate(id delegate) const { _setDelegate(self,delegate); }

        void finishLaunching() const { _finishLaunching(self); }

        id nextEvent(id untilDate) const {
            enum { NSAnyEventMask = 0xffffffffu };
            return _nextEventMatchingMask_untilDate_inMode_dequeue(
                self,NSAnyEventMask,untilDate,NSDefaultRunLoopMode,true
            );
        }

        void sendEvent(id event) const { _sendEvent(self,event); }

        void terminate() const { _terminate(self,self); }

        void updateWindows() const { _updateWindows(self); }

    } const application {};

    //--------------------------------------------------------------------------

    class NSDate {
        objc::class_id const cls {"NSDate"};

        objc::message<id(double)>
        _dateWithTimeIntervalSinceNow
        {"dateWithTimeIntervalSinceNow:"};

    public:

        NSDate() = default;

        id withTimeIntervalSinceNow(double seconds) const {
            return _dateWithTimeIntervalSinceNow(cls,seconds);
        }

    } const date {};

    //--------------------------------------------------------------------------

    class NSNotificationCenter {
        id const self {
            objc::message<id()>{"defaultCenter"}(
                objc::class_id("NSNotificationCenter")
            )
        };

        objc::message<void(id,id)>
        _postNotificationName_object
        {"postNotificationName:object:"};

    public:

        NSNotificationCenter() = default;

        void applicationDidFinishLaunchingNotification() const {
            _postNotificationName_object(
                self,NSApplicationDidFinishLaunchingNotification,application
            );
        }

        void applicationWillTerminateNotification() const {
            _postNotificationName_object(
                self,NSApplicationWillTerminateNotification,application
            );
        }
    } const notificationCenter {};

    //--------------------------------------------------------------------------

    static std::atomic<bool> terminated;

    static const objc::class_id AppDelegate {
        "AppDelegate","NSObject",

        objc::method {
            "applicationWillFinishLaunching:",
            [](id self, objc::selector _cmd, id notification) {
                puts(_cmd.name());
            }
        },

        objc::method {
            "applicationDidFinishLaunching:",
            [](id self, objc::selector _cmd, id notification) {
                puts(_cmd.name());
            }
        },

        objc::method {
            "applicationShouldTerminate:",
            [](id self, objc::selector _cmd, id application) {
                puts(_cmd.name());
                enum : uintptr_t {
                    NSTerminateCancel = 0,
                    NSTerminateNow    = 1,
                    NSTerminateLater  = 2,
                };
                terminated.store(true);
                return NSTerminateCancel;
            }
        },

        objc::method {
            "applicationWillTerminate:",
            [](id self, objc::selector _cmd, id notification) {
                puts(_cmd.name());
            }
        },
    };

}} // namespace app::macos

//------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    using namespace app::macos;

    objc::autoreleasepool autoreleasepool;

    application.setDelegate(objc::alloc(AppDelegate));
    application.finishLaunching();
    notificationCenter.applicationDidFinishLaunchingNotification();
    const int status = app::main(argc,argv);
    notificationCenter.applicationWillTerminateNotification();

    puts("DONE");
    return status;
}

//------------------------------------------------------------------------------

bool app::update(const float timeout_ms) {
    using namespace app::macos;

    objc::autoreleasepool autoreleasepool;

    objc::unique_ptr timeout_date {
        (timeout_ms > 0)
        ? date.withTimeIntervalSinceNow(timeout_ms / 1000.0)
        : nullptr
    };

    if (not terminated.load()) {
        while (id event = application.nextEvent(timeout_date.get())) {
            application.sendEvent(event);
            application.updateWindows();
        }
    }
    return not terminated.load();
}

//------------------------------------------------------------------------------

void app::quit() {
    using namespace app::macos;

    application.terminate();
}

//------------------------------------------------------------------------------

int app::main(int argc, char* argv[]) {
    int counter = 0;
    while (app::update(0)) {
        puts("waiting...");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/10));
        if (++counter > 10) {
            app::quit();
        }
    }
    return 0;
}
