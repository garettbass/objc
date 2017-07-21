#pragma once

#include "detail/platform.hpp"
#if OBJC_OS_APPLE

#include <cassert>
#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#ifndef OBJC_NAMESPACE
#define OBJC_NAMESPACE objc
#endif // OBJC_NAMESPACE

#include "detail/function_traits.hpp"
#include "detail/library.hpp"
#include "detail/CoreFoundation.hpp"
#include "detail/Foundation.hpp"

namespace OBJC_NAMESPACE {

    #if defined(__LP64__) && __LP64__
        using CGFloat    = double;
        using NSUInteger = unsigned long;
        using NSInteger  = signed long;
    #else
        using CGFloat    = float;
        using NSUInteger = unsigned int;
        using NSInteger  = signed int;
    #endif

    //--------------------------------------------------------------------------

    using class_t = Class;

    using id_t = id;

    using imp_t = IMP;

    using ivar_t = Ivar;

    using object_t = objc_object;

    using protocol_t = Protocol;

    using sel_t = SEL;

    using super_t = objc_super;

    //--------------------------------------------------------------------------

    struct object {
        id_t const id = nullptr;

        object(decltype(nullptr)): id(nullptr) {}

        object(id_t obj): id(obj) {}

        object(const struct class_id& cls): id((id_t&)(cls)) {}

        explicit object(void* obj): id(id_t(obj)) {}

        explicit object(uintptr_t obj): id(id_t(obj)) {}

        explicit operator bool() const { return id != nullptr; }

        explicit operator size_t() const { return size_t(id); }

        operator id_t() const { return id; }

        bool operator ==(const object& o) const { return id == o.id; }
        bool operator !=(const object& o) const { return id != o.id; }
    };

    //--------------------------------------------------------------------------

    struct protocol {
        protocol_t* const proto = nullptr;

        protocol(decltype(nullptr)): proto(nullptr) {}

        protocol(protocol_t* const proto)
        : proto(proto)
        {}

        protocol(const char* name)
        : proto(objc_getProtocol(name))
        {}

        explicit operator bool() const { return proto != nullptr; }

        operator protocol_t*() const { return proto; }

        bool operator ==(const protocol& p) const { return proto == p.proto; }
        bool operator !=(const protocol& p) const { return proto != p.proto; }

        const char* name() const {
            return proto ? protocol_getName(proto): nullptr;
        }
    };

    //--------------------------------------------------------------------------

    struct selector {
        sel_t const sel = nullptr;

        selector(decltype(nullptr)): sel(nullptr) {}

        selector(sel_t sel): sel(sel) {}

        selector(const char* name): sel(sel_getUid(name)) {}

        explicit operator bool() const { return sel != nullptr; }

        operator sel_t() const { return sel; }

        bool operator ==(const selector& s) const { return sel == s.sel; }
        bool operator !=(const selector& s) const { return sel != s.sel; }

        const char* name() const { return sel ? sel_getName(sel): nullptr; }
    };

    //--------------------------------------------------------------------------

    struct super : super_t {
        super(id_t self, class_t super)
        : super_t{self,super} {}

        explicit
        super(object obj)
        : super(obj,class_getSuperclass(object_getClass(obj))) {}

        explicit operator bool() const { return super_class != nullptr; }

        operator super_t*() { return this; }
    };

    //--------------------------------------------------------------------------

    template<typename Value>
    struct variable {
        const char* const name = nullptr;

        variable(const char* name)
        : name(name) {}
    };

    //--------------------------------------------------------------------------

    struct method {
        sel_t const sel = nullptr;
        imp_t const imp = nullptr;

        method(selector sel, imp_t imp)
        : sel(sel), imp(imp) {}

        template<typename Callback>
        method(selector sel, Callback&& callback)
        : method(sel, imp_t(function_cast(callback))) {
            using arg0_t = argument_type<Callback,0>;
            static_assert(
                std::is_same<arg0_t,object>::value or
                std::is_pointer<arg0_t>::value,
                "first argument must be of type 'id' or 'objc::object'"
            );
            using arg1_t = argument_type<Callback,1>;
            static_assert(
                std::is_same<arg1_t,SEL>::value or
                std::is_same<arg1_t,selector>::value,
                "second argument must be of type 'SEL' or 'objc::selector'"
            );
        }
    };

    //--------------------------------------------------------------------------

    struct class_id {
        class_t const cls = nullptr;

        class_id(decltype(nullptr)): cls(nullptr) {}

        class_id(class_t cls): cls(cls) {}

        class_id(const char* name): cls(objc_getClass(name)) {}

        class_id(id_t obj): cls(object_getClass(obj)) {}

        template<typename... Defs>
        class_id(const char* name, class_id super, Defs&&... defs)
        : class_id(objc_allocateClassPair(super,name,0)) {
            assert(cls||!"failed to allocate class");
            struct local { static void unpack(...) {} };
            local::unpack(((define(super,defs)),'\0')...);
            objc_registerClassPair(cls);
        }

        explicit operator bool() const { return cls != nullptr; }

        operator class_t() { return cls; }

        bool operator ==(const class_id& c) const { return cls == c.cls; }
        bool operator !=(const class_id& c) const { return cls != c.cls; }

        const char* name() const { return cls ? class_getName(cls) : nullptr; }

    private:

        template<typename T>
        void define(class_id super, const variable<T>& v) {
            if (not class_addIvar(cls,v.name,sizeof(T),0,"?")) {
                assert(!"failed to add variable");
            }
        }

        void define(class_id super, const method& m) {
            if (not class_addMethod(cls,m.sel,m.imp,"@:")) {
                assert(!"failed to add method");
            }
        }

        void define(class_id super, const protocol& p) {
            if (not class_addProtocol(cls,p)) {
                assert(!"failed to add protocol");
            }
        }
    };

    //--------------------------------------------------------------------------

    inline
    class_id classof(id_t obj) { return { obj }; }

    inline
    class_id classof(super super) { return { super.super_class }; }

    //--------------------------------------------------------------------------

    inline
    long get_retain_count(object obj) { return CFGetRetainCount(obj); }

    inline
    void retain(object obj) { CFRetain(obj); }

    inline
    void release(object obj) { CFRelease(obj); }

    //--------------------------------------------------------------------------

    template<typename T>
    struct accessor {
        ivar_t const ivar   = nullptr;
        size_t const offset = 0;

        accessor(class_id cls, const char* name)
        : ivar(class_getInstanceVariable(cls,name))
        , offset(ivar ? ivar_getOffset(ivar): 0) {}

        explicit operator bool() const { return ivar != nullptr; }

        T& operator ()(object obj) const {
            assert(ivar);
            return *((T*)(ivar ? (size_t(obj) + offset) : size_t(0)));
        }

        template<typename Self>
        T& operator ()(Self* obj) const {
            assert(ivar);
            return *((T*)(ivar ? (size_t(obj) + offset) : size_t(0)));
        }
    };

    template<typename T>
    T get(object obj, const char* name) {
        assert(obj);
        if (obj) {
            accessor<T> access { classof(obj),name };
            assert(access);
            if (access) {
                return access(obj);
            }
        }
        return T{};
    }

    template<typename T>
    void set(object obj, const char* name, const T& value) {
        assert(obj);
        if (obj) {
            accessor<T> access { classof(obj),name };
            assert(access);
            if (access) {
                access(obj) = value;
            }
        }
    }

    //--------------------------------------------------------------------------

    template<typename>
    struct implementation;

    template<typename Result, typename... Args>
    struct implementation<Result(Args...)> {
        using imp_t = Result(*)(id,sel_t,Args...);

        sel_t const sel = nullptr;
        imp_t const imp = nullptr;

        implementation(class_id cls, selector sel)
        : sel(sel), imp(imp_t(class_getMethodImplementation(cls,sel))) {}

        Result operator ()(object obj, Args... args) const {
            return imp(obj,sel,std::forward<Args>(args)...);
        }

        template<typename Self>
        Result operator ()(Self* obj, Args... args) const {
            return imp(id_t(obj),sel,std::forward<Args>(args)...);
        }
    };

    //--------------------------------------------------------------------------

    #include "detail/message_base.hpp"

    template<typename>
    struct message;

    template<typename Result, typename... Args>
    struct message<Result(Args...)> : message_base<Result(Args...)> {
        sel_t const sel = nullptr;

        message(selector sel)
        : sel(sel) {}

        Result operator ()(object obj, Args... args) const {
            return send(id_t(obj),sel,std::forward<Args>(args)...);
        }

        Result operator ()(super sup, Args... args) const {
            return send((super_t&)(sup),sel,std::forward<Args>(args)...);
        }

        template<typename Self>
        Result operator ()(Self* obj, Args... args) const {
            return send(id_t(obj),sel,std::forward<Args>(args)...);
        }

        using message_base<Result(Args...)>::send;
    };

    //--------------------------------------------------------------------------

    inline
    id alloc(class_id cls) {
        message<id()> alloc { "alloc" };
        message<id()> init { "init" };
        return init(alloc(cls));
    }

    template<typename... Args>
    id alloc(class_id cls, selector sel, Args... args) {
        message<id()> alloc { "alloc" };
        message<id(Args...)> init { sel };
        return init(alloc(cls),std::forward<Args>(args)...);
    }

    //--------------------------------------------------------------------------

    struct CGPoint { CGFloat x,y; };
    struct CGSize  { CGFloat width,height; };
    struct CGRect  { CGPoint origin; CGSize size; };

    //--------------------------------------------------------------------------

    using NSPoint = CGPoint;
    using NSSize  = CGSize;
    using NSRect  = CGRect;

    //--------------------------------------------------------------------------

    struct NSObject {

        void* operator new(size_t)             = delete;
        void  operator delete(void* p)         { ((NSObject*)p)->release(); }
        NSObject()                             = delete;
        NSObject(NSObject&&)                   = delete;
        NSObject(const NSObject&)              = delete;
        NSObject& operator = (NSObject&&)      = delete;
        NSObject& operator = (const NSObject&) = delete;

        static struct API {

            class_id cls {"NSObject"};

            message<NSObject*()>
            alloc {"alloc"};

            message<NSObject*()>
            autorelease {"autorelease"};

            message<void()>
            dealloc {"dealloc"};

            message<NSObject*()>
            init {"init"};

            message<void()>
            release {"release"};

            message<NSObject*()>
            retain {"retain"};

            message<bool(Class)>
            isKindOfClass {"isKindOfClass:"};

        } api;

    protected:

        template<typename ObjectType>
        static
        ObjectType*
        alloc() {
            return (ObjectType*)api.alloc(ObjectType::api.cls);
        }

        template<typename ObjectType>
        ObjectType*
        init() { return (ObjectType*)api.init(this); }

    public:

        static
        NSObject*
        alloc() { return NSObject::alloc<NSObject>(); }

        NSObject*
        init() { return NSObject::init<NSObject>(); }

        template<typename ObjectType>
        ObjectType*
        as() { return is<ObjectType>() ? (ObjectType*)this : nullptr; }

        NSObject*
        autorelease() { return api.autorelease(this); }

        template<typename ObjectType>
        bool
        is() const { return isKindOfClass(ObjectType::api.cls); }

        bool
        isKindOfClass(class_id cls) const {
            return api.isKindOfClass(this,cls);
        }

        void
        release() { api.release(this); }

        NSObject*
        retain() { return api.retain(this); }

    };

    NSObject::API NSObject::api {};

    //--------------------------------------------------------------------------

    struct NSAutoreleasePool : NSObject {

        static struct API {

            class_id cls {"NSAutoreleasePool"};

        } api;

        static
        NSAutoreleasePool*
        alloc() { return NSObject::alloc<NSAutoreleasePool>(); }

        NSAutoreleasePool*
        init() { return NSObject::init<NSAutoreleasePool>(); }

    };

    NSAutoreleasePool::API NSAutoreleasePool::api {};

    //--------------------------------------------------------------------------

    struct autoreleasepool {
        NSAutoreleasePool* const pool = NSAutoreleasePool::alloc()->init();
        autoreleasepool() = default;
        autoreleasepool(decltype(nullptr)) : pool(nullptr) {}
       ~autoreleasepool() { delete pool; new(this) autoreleasepool(nullptr); }
    };

    //--------------------------------------------------------------------------

    template<typename = NSObject>
    struct NSArray;

    template<>
    struct NSArray<NSObject> : NSObject {

        static struct API {

            class_id cls {"NSArray"};

            message<NSArray*(NSObject**,NSUInteger)>
            arrayWithObjects_count {"arrayWithObjects:count:"};

            message<NSUInteger()>
            count {"count"};

            message<NSObject*(NSUInteger)>
            objectAtIndex {"objectAtIndex:"};

        } api;

        template<typename... ObjectTypes>
        static
        NSArray*
        array(ObjectTypes... objects) {
            enum : NSUInteger { count = sizeof...(ObjectTypes) };
            NSObject* array[count] { objects... };
            return api.arrayWithObjects_count(api.cls,array,count);
        }

        NSUInteger count() const { return api.count(this); }

        NSObject* objectAtIndex(NSUInteger index) const {
            return api.objectAtIndex(this,index);
        }

    };

    template<typename ObjectType>
    struct NSArray : NSArray<NSObject> {

        template<typename... ObjectTypes>
        static
        NSArray*
        array(ObjectTypes... objects) {
            enum : NSUInteger { count = sizeof...(ObjectTypes) };
            ObjectType* array[count] { objects... };
            return api.arrayWithObjects_count(api.cls,array,count);
        }

        ObjectType* objectAtIndex(NSUInteger index) const {
            return (ObjectType*)api.objectAtIndex(this,index);
        }

    };

    NSArray<NSObject>::API NSArray<NSObject>::api {};

    //--------------------------------------------------------------------------

    struct NSString : NSObject {

        static struct API {

            class_id cls {"NSString"};

            message<NSString*(const char*)>
            stringWithUTF8String {"stringWithUTF8String:"};

            message<const char*()>
            UTF8String {"UTF8String"};

        } api;

        static
        NSString*
        string(const char* utf8String) {
            return api.stringWithUTF8String(api.cls,utf8String);
        }

        const char*
        UTF8String() const {
            return api.UTF8String(this);
        }

    };

    NSString::API NSString::api {};

} // namespace OBJC_NAMESPACE

#endif // OBJC_OS_APPLE