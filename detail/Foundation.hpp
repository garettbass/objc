#pragma once

namespace OBJC_NAMESPACE {

    library Foundation {"Foundation.framework/Foundation"};

    struct objc_class;
    struct objc_ivar;
    struct objc_object;
    struct objc_selector;
    struct objc_super { objc_object* receiver; objc_class* super_class; };

    using Class    = objc_class*;
    using id       = objc_object*;
    using IMP      = function_t<void*(objc_object*,objc_selector*)>;
    using Ivar     = objc_ivar*;
    using Protocol = objc_object;
    using SEL      = objc_selector*;

    function_t<bool(Class,const char*,size_t,uint8_t,const char*)>
    const class_addIvar {
        Foundation("class_addIvar")
    };

    function_t<bool(Class,SEL,IMP,const char*)>
    const class_addMethod {
        Foundation("class_addMethod")
    };

    function_t<bool(Class,Protocol*)>
    const class_addProtocol {
        Foundation("class_addProtocol")
    };

    function_t<Ivar(Class,const char*)>
    const class_getInstanceVariable {
        Foundation("class_getInstanceVariable")
    };

    function_t<IMP(Class,SEL)>
    const class_getMethodImplementation {
        Foundation("class_getMethodImplementation")
    };

    function_t<const char*(Class cls)>
    const class_getName {
        Foundation("class_getName")
    };

    function_t<Class(Class cls)>
    const class_getSuperclass {
        Foundation("class_getSuperclass")
    };

    function_t<ptrdiff_t(Ivar)>
    const ivar_getOffset {
        Foundation("ivar_getOffset")
    };

    function_t<void(void*)>
    const objc_autoreleasePoolPop {
        Foundation("objc_autoreleasePoolPop")
    };

    function_t<void*()>
    const objc_autoreleasePoolPush {
        Foundation("objc_autoreleasePoolPush")
    };

    function_t<Class(Class,const char*,size_t)>
    const objc_allocateClassPair {
        Foundation("objc_allocateClassPair")
    };

    function_t<Class(const char*)>
    const objc_getClass {
        Foundation("objc_getClass")
    };

    function_t<Protocol*(const char*)>
    const objc_getProtocol {
        Foundation("objc_getProtocol")
    };

    function_t<id(id,SEL)>
    const objc_msgSend {
        Foundation("objc_msgSend")
    };

    function_t<id(objc_super*,SEL)>
    const objc_msgSendSuper {
        Foundation("objc_msgSendSuper")
    };

    function_t<long double(id,SEL)>
    const objc_msgSend_fpret {
        #if CXX_CPU_X86
            Foundation("objc_msgSend_fpret")
        #elif CXX_CPU_ARM
            Foundation("objc_msgSend")
        #else
            #error "objc_msgSend_fpret unresolved"
        #endif
    };

    function_t<void(id,SEL)>
    const objc_msgSend_stret {
        #if CXX_CPU_X86
            Foundation("objc_msgSend_stret")
        #elif CXX_CPU_ARM
            Foundation("objc_msgSend")
        #else
            #error "objc_msgSend_stret unresolved"
        #endif
    };

    function_t<void(objc_super*,SEL)>
    const objc_msgSendSuper_stret {
        #if CXX_CPU_X86
            Foundation("objc_msgSendSuper_stret")
        #elif CXX_CPU_ARM
            Foundation("objc_msgSendSuper")
        #else
            #error "objc_msgSendSuper_stret unresolved"
        #endif
    };

    function_t<void(Class cls)>
    const objc_registerClassPair {
        Foundation("objc_registerClassPair")
    };

    function_t<Class(id obj)>
    const object_getClass {
        Foundation("object_getClass")
    };

    function_t<const char*(Protocol*)>
    const protocol_getName {
        Foundation("protocol_getName")
    };

    function_t<const char*(SEL)>
    const sel_getName {
        Foundation("sel_getName")
    };

    function_t<SEL(const char*)>
    const sel_getUid {
        Foundation("sel_getUid")
    };

} // namespace OBJC_NAMESPACE