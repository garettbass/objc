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

classid AppDelegate {
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