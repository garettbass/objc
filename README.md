# An Objective-C API for C++

[![Build Status](https://travis-ci.org/garettbass/objc.svg?branch=master)](https://travis-ci.org/garettbass/objc)

## Include the Header

``` C++
// optional: embed objc namespace in another namespace...
// #define OBJC_PARENT_NAMESPACE mycoolnamespace
#include <objc/objc.inl>
```

## Call Methods

``` C++
struct NSPoint { double x, y; };

// 1. easily call a method by dynamic dispatch
objc::call<void>(window,"setFrameOrigin:",NSPoint{0,0});

// 2. store a type-safe interface to call a method by dynamic dispatch
objc::method_interface<void(NSPoint)> setFrameOrigin {
    "setFrameOrigin:"
};
setFrameOrigin(window,{0,0});

// 3. store a type-safe pointer to a specific method implementation
objc::method_implementation<void()> NSWindow_setFrameOrigin {
    "NSWindow","setFrameOrigin:"
};
NSWindow_setFrameOrigin(window,{0,0});
```

## Define Classes

``` C++
using namespace objc; // reduce verbosity!

class_definition AppDelegate {
    "AppDelegate", // class name
    "NSObject",    // superclass name

    // define a member variable
    member<int>("goes_to"),

    // define an init method
    method("init",[](id self,SEL init){
        // delegate to [super init]
        self = call<id>(super(self),init);
        // initialize member variable
        member<int>(self,"goes_to") = 11;
        return self;
    }),

    // define some more methods...

    method("applicationDidFinishLaunching:",[](id self,SEL,id notification){
        std::cout << "application did finish launching :) \n";
    }),

    method("applicationWillTerminate:",[](id self,SEL,id notification){
        std::cout << "application will terminate :( \n";
    }),
};
```

## Instantiate Classes

``` C++
// [[AppDelegate alloc] init]
id delegate = objc::alloc(AppDelegate);
// do stuff ...
objc::release(delegate);
```

``` C++
id delegate = objc::alloc("AppDelegate"); // string is OK
```

``` C++
// make a std::unique_ptr<id,objc::deleter>...
objc::unique_ptr uniqueDelegatePtr = objc::make_unique(AppDelegate);
```

``` C++
// make a std::shared_ptr<id,objc::deleter>...
objc::shared_ptr sharedDelegatePtr = objc::make_shared(AppDelegate);
```

``` C++
// do some serious initialization
objc::class_id NSWindow { "NSWindow" };

struct NSPoint { double x, y; };
struct NSSize { double width, height; };
struct NSRect { NSPoint origin; NSSize size; };

enum NSWindowStyleMask : unsigned long {
    Borderless             = 0,
    Titled                 = 1 << 0,
    Closable               = 1 << 1,
    Miniaturizable         = 1 << 2,
    Resizable              = 1 << 3,
    UtilityWindow          = 1 << 4,
    DocModalWindow         = 1 << 6,
    NonactivatingPanel     = 1 << 7,
    TexturedBackground     = 1 << 8,
    UnifiedTitleAndToolbar = 1 << 12,
    HUDWindow              = 1 << 13,
    FullScreen             = 1 << 14,
    FullSizeContentView    = 1 << 15,
}

enum NSBackingStore : unsigned long {
    Retained     = 0, // deprecated
    Nonretained  = 1, // deprecated
    Buffered     = 2,
};

// [[NSWindow alloc] initWithContentRect:...]
id window = objc::alloc(
    NSWindow,
    "initWithContentRect:styleMask:backing:defer:",
    NSRect{{0,0},{800,600}},
    NSWindowStyleMask(
        NSWindowStyleMask::Titled |
        NSWindowStyleMask::Closable |
        NSWindowStyleMask::Miniaturizable |
        NSWindowStyleMask::Resizable
    ),
    NSBackingStore::Buffered,
    true
);
```