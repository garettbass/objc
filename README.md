# An Objective-C API for C++

[![Build Status](https://travis-ci.org/garettbass/objc.svg?branch=master)](https://travis-ci.org/garettbass/objc)

``` C++
#include <objc/objc> // or <objc/objc.hpp>
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
objc::class_definition AppDelegate {
    "AppDelegate", // class name
    "NSObject",    // superclass name

    objc::protocol("NSApplicationDelegate"),

    objc::member<int>("awesomeness"), // a member variable

    objc::method("init",[](id self, SEL){
        self = objc::call<id>(objc::super(self),"init");
        objc::member<int>(self,"awesomeness") = 11;
        return self;
    }),

    objc::method("applicationDidFinishLaunching:",[](id self,SEL,id notification){
        std::cout << "application did finish launching :) \n";
    }),

    objc::method("applicationWillTerminate:",[](id self,SEL,id notification){
        std::cout << "application will terminate :( \n";
    }),
};
```

## Instantiate Classes

``` C++
id delegate = objc::alloc(AppDelegate);

objc::class_id NSWindow { "NSWindow" };

objc::unique_ptr windowPtr = objc::make_unique(NSWindow);
```