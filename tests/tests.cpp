#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "../objc.hpp"

using namespace Catch;

TEST_CASE("objc::class_id") {
    objc::class_id NSObject { "NSObject" };
    REQUIRE(NSObject.cls != nullptr);
    REQUIRE_THAT(NSObject.name(),Equals("NSObject"));
}

TEST_CASE("objc::protocol") {
    objc::protocol NSTextViewDelegate { "NSTextViewDelegate" };
    REQUIRE(NSTextViewDelegate.proto != nullptr);
    REQUIRE_THAT(NSTextViewDelegate.name(),Equals("NSTextViewDelegate"));
}

TEST_CASE("objc::selector") {
    objc::selector alloc { "alloc" };
    REQUIRE(alloc.sel != nullptr);
    REQUIRE_THAT(alloc.name(),Equals("alloc"));
}

TEST_CASE("objc::super") {
    objc::class_id NSObject { "NSObject" };
    objc::class_definition TestClass1 { "TestClass1",NSObject };
    REQUIRE(TestClass1.cls != nullptr);
    REQUIRE_THAT(TestClass1.name(),Equals("TestClass1"));
    auto obj = objc::alloc(TestClass1);
    auto super = objc::super(obj);
    REQUIRE(objc::classof(super) == NSObject);
}

TEST_CASE("objc::retain/release") {
    static bool initialized = false;
    static bool deallocated = false;

    objc::class_definition TestClass2 {
        "TestClass2","NSObject",
        objc::method("init",[](id self,SEL sel){
            self = objc::call<id>(objc::super(self),sel);
            initialized = true;
        }),
        objc::method("dealloc",[](id self,SEL sel){
            deallocated = true;
            objc::call<void>(objc::super(self),sel);
        })
    };

    REQUIRE(not initialized);
    REQUIRE(not deallocated);

    objc::object obj = alloc(TestClass2);
    REQUIRE(initialized);
    REQUIRE(not deallocated);
    REQUIRE(get_retain_count(obj) == 1);

    objc::retain(obj);
    REQUIRE(get_retain_count(obj) == 2);

    objc::release(obj);
    REQUIRE(get_retain_count(obj) == 1);

    objc::release(obj);
    REQUIRE(deallocated);
}

int main(int argc, char * argv[]) {
    objc::autoreleasepool autoreleasepool;

    Catch::Session session;

    const int commandLineError = session.applyCommandLine(argc, argv);
    if (commandLineError) {
        return commandLineError;
    }

    const int testFailureCount = session.run();
    return std::min(testFailureCount,255);
}
