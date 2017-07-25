#include <algorithm>
#include <string>
#include "../objc.inl"

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.hpp"
using namespace doctest;

#if OBJC_OS_APPLE

using objc::id;
using objc::SEL;

TEST_CASE("objc::classid") {
    objc::classid NSObject { "NSObject" };
    REQUIRE(NSObject.cls != nullptr);
    REQUIRE(NSObject.name() == std::string("NSObject"));
}

TEST_CASE("objc::selector") {
    objc::selector alloc { "alloc" };
    REQUIRE(alloc.sel != nullptr);
    REQUIRE(alloc.name() == std::string("alloc"));
}

TEST_CASE("objc::super") {
    objc::classid NSObject { "NSObject" };
    objc::classid TestClass1 { "TestClass1",NSObject };
    REQUIRE(TestClass1.cls != nullptr);
    REQUIRE(TestClass1.name() == std::string("TestClass1"));
    auto obj = objc::alloc(TestClass1);
    auto super = objc::super(obj);
    REQUIRE(objc::classof(super) == NSObject);
}

TEST_CASE("objc::retain/release") {
    static bool initialized = false;
    static bool deallocated = false;

    objc::classid TestClass2 {
        "TestClass2","NSObject",
        objc::method("init",[](id self,SEL sel){
            self = objc::message<id()>{sel}(objc::super(self));
            initialized = true;
        }),
        objc::method("dealloc",[](id self,SEL sel){
            deallocated = true;
            objc::message<void()>{sel}(objc::super(self));
        })
    };

    REQUIRE(not initialized);
    REQUIRE(not deallocated);

    objc::object obj = alloc(TestClass2);
    REQUIRE(initialized);
    REQUIRE(not deallocated);
    REQUIRE(objc::CFGetRetainCount(obj) == 1);

    objc::CFRetain(obj);
    REQUIRE(objc::CFGetRetainCount(obj) == 2);

    objc::CFRelease(obj);
    REQUIRE(objc::CFGetRetainCount(obj) == 1);

    objc::CFRelease(obj);
    REQUIRE(deallocated);
}

#endif // OBJC_OS_APPLE

int main(int argc, char * argv[]) {
    #if OBJC_OS_APPLE
    objc::autoreleasepool autoreleasepool;
    #endif // OBJC_OS_APPLE
    Context context { argc, argv };
    const int failures = context.run();
    return std::min(failures,255);
}
