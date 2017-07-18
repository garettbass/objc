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
    using IMP      = function_type<void*(objc_object*,objc_selector*)>;
    using Ivar     = objc_ivar*;
    using Protocol = objc_object;
    using SEL      = objc_selector*;

    function_type<bool(Class,const char*,size_t,uint8_t,const char*)>
    const class_addIvar {
        Foundation.function("class_addIvar")
    };

    function_type<bool(Class,SEL,IMP,const char*)>
    const class_addMethod {
        Foundation.function("class_addMethod")
    };

    function_type<bool(Class,Protocol*)>
    const class_addProtocol {
        Foundation.function("class_addProtocol")
    };

    function_type<Ivar(Class,const char*)>
    const class_getInstanceVariable {
        Foundation.function("class_getInstanceVariable")
    };

    function_type<IMP(Class,SEL)>
    const class_getMethodImplementation {
        Foundation.function("class_getMethodImplementation")
    };

    function_type<const char*(Class cls)>
    const class_getName {
        Foundation.function("class_getName")
    };

    function_type<Class(Class cls)>
    const class_getSuperclass {
        Foundation.function("class_getSuperclass")
    };

    function_type<ptrdiff_t(Ivar)>
    const ivar_getOffset {
        Foundation.function("ivar_getOffset")
    };

    function_type<Class(Class,const char*,size_t)>
    const objc_allocateClassPair {
        Foundation.function("objc_allocateClassPair")
    };

    function_type<Class(const char*)>
    const objc_getClass {
        Foundation.function("objc_getClass")
    };

    function_type<Protocol*(const char*)>
    const objc_getProtocol {
        Foundation.function("objc_getProtocol")
    };

    function_type<id(id,SEL)>
    const objc_msgSend {
        Foundation.function("objc_msgSend")
    };

    function_type<id(objc_super*,SEL)>
    const objc_msgSendSuper {
        Foundation.function("objc_msgSendSuper")
    };


    function_type<long double(id,SEL)>
    const objc_msgSend_fpret {
        #if OBJC_CPU_X86
            Foundation.function("objc_msgSend_fpret")
        #elif OBJC_CPU_ARM
            Foundation.function("objc_msgSend")
        #else
            #error "objc_msgSend_fpret unresolved"
        #endif
    };

    function_type<void(id,SEL)>
    const objc_msgSend_stret {
        #if OBJC_CPU_X86
            Foundation.function("objc_msgSend_stret")
        #elif OBJC_CPU_ARM
            Foundation.function("objc_msgSend")
        #else
            #error "objc_msgSend_stret unresolved"
        #endif
    };

    function_type<void(objc_super*,SEL)>
    const objc_msgSendSuper_stret {
        #if OBJC_CPU_X86
            Foundation.function("objc_msgSendSuper_stret")
        #elif OBJC_CPU_ARM
            Foundation.function("objc_msgSendSuper")
        #else
            #error "objc_msgSendSuper_stret unresolved"
        #endif
    };

    function_type<void(Class cls)>
    const objc_registerClassPair {
        Foundation.function("objc_registerClassPair")
    };

    function_type<Class(id obj)>
    const object_getClass {
        Foundation.function("object_getClass")
    };

    function_type<const char*(Protocol*)>
    const protocol_getName {
        Foundation.function("protocol_getName")
    };

    function_type<const char*(SEL)>
    const sel_getName {
        Foundation.function("sel_getName")
    };

    function_type<SEL(const char*)>
    const sel_getUid {
        Foundation.function("sel_getUid")
    };

} // namespace OBJC_NAMESPACE