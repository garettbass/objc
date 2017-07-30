#pragma once
#include <cassert>
#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "dll/cxx/push.h"
#include "dll/cxx/cxx.h"
#if CXX_OS_APPLE

#ifndef OBJC_NAMESPACE
#define OBJC_NAMESPACE objc
#endif // OBJC_NAMESPACE

#define DLL_NAMESPACE OBJC_NAMESPACE
#include "dll/dll.hpp"

#define FUNCTION_TRAITS_NAMESPACE OBJC_NAMESPACE
#include "function_traits/function_traits.hpp"

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

    struct selector {
        SEL const sel = nullptr;

        selector(decltype(nullptr)): sel(nullptr) {}

        selector(SEL sel): sel(sel) {}

        selector(const char* name): sel(sel_getUid(name)) {}

        explicit operator bool() const { return sel != nullptr; }

        operator SEL() const { return sel; }

        bool operator ==(const selector& s) const { return sel == s.sel; }
        bool operator !=(const selector& s) const { return sel != s.sel; }

        const char* name() const { return sel ? sel_getName(sel): nullptr; }
    };

    //--------------------------------------------------------------------------

    struct object {
        objc_object* const id = nullptr;

        object(decltype(nullptr)): id(nullptr) {}

        object(objc_object* obj): id(obj) {}

        object(const struct classid& cls): id((objc_object*&)(cls)) {}

        explicit object(void* obj): id((objc_object*)obj) {}

        explicit operator bool() const { return id != nullptr; }

        explicit operator size_t() const { return size_t(id); }

        operator objc_object*() const { return id; }

        bool operator ==(const object& o) const { return id == o.id; }
        bool operator !=(const object& o) const { return id != o.id; }
    };

    //--------------------------------------------------------------------------

    struct super : objc_super {
        super(objc_object* self, objc_class* super)
        : objc_super{self,super} {}

        explicit
        super(object obj)
        : super(obj,class_getSuperclass(object_getClass(obj))) {}

        explicit operator bool() const { return super_class != nullptr; }

        operator objc_super*() { return this; }
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
        SEL const sel = nullptr;
        IMP const imp = nullptr;

        method(selector sel, IMP imp)
        : sel(sel), imp(imp) {}

        template<typename Callback>
        method(selector sel, Callback&& callback)
        : method(sel, IMP(function_t<Callback>(callback))) {
            using first_parameter_t = parameter_t<0,Callback>;
            static_assert(
                std::is_pointer<first_parameter_t>::value or
                std::is_same<first_parameter_t,object>::value,
                "first parameter must be of type 'id' or 'objc::object'"
            );
            using second_parameter_t = parameter_t<1,Callback>;
            static_assert(
                std::is_same<second_parameter_t,SEL>::value or
                std::is_same<second_parameter_t,selector>::value,
                "second parameter must be of type 'SEL' or 'objc::selector'"
            );
        }
    };

    //--------------------------------------------------------------------------

    struct classid {
        objc_class* const cls = nullptr;

        classid(decltype(nullptr)): cls(nullptr) {}

        classid(objc_class* cls): cls(cls) {}

        classid(const char* name): cls(objc_getClass(name)) {}

        classid(objc_object* obj): cls(object_getClass(obj)) {}

        template<typename... Defs>
        classid(const char* name, classid super, Defs&&... defs)
        : classid(objc_allocateClassPair(super,name,0)) {
            assert(cls||!"failed to allocate class");
            struct local { static void unpack(...) {} };
            local::unpack(((define(super,defs)),'\0')...);
            objc_registerClassPair(cls);
        }

        explicit operator bool() const { return cls != nullptr; }

        operator objc_class*() { return cls; }

        bool operator ==(const classid& c) const { return cls == c.cls; }
        bool operator !=(const classid& c) const { return cls != c.cls; }

        const char* name() const { return cls ? class_getName(cls) : nullptr; }

    private:

        template<typename T>
        void define(classid super, const variable<T>& v) {
            if (not class_addIvar(cls,v.name,sizeof(T),0,"?")) {
                assert(!"failed to add variable");
            }
        }

        void define(classid super, const method& m) {
            if (not class_addMethod(cls,m.sel,m.imp,"@:")) {
                assert(!"failed to add method");
            }
        }
    };

    //--------------------------------------------------------------------------

    inline
    classid classof(objc_object* obj) { return { obj }; }

    inline
    classid classof(super super) { return { super.super_class }; }

    //--------------------------------------------------------------------------

    template<typename T>
    struct accessor {
        Ivar   const ivar   = nullptr;
        size_t const offset = 0;

        accessor(classid cls, const char* name)
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
        using IMP = Result(*)(id,SEL,Args...);

        SEL const sel = nullptr;
        IMP const imp = nullptr;

        implementation(classid cls, selector sel)
        : sel(sel), imp(IMP(class_getMethodImplementation(cls,sel))) {}

        Result operator ()(object obj, Args... args) const {
            return imp(obj,sel,args...);
        }

        template<typename Self>
        Result operator ()(Self* obj, Args... args) const {
            return imp((objc_object*)obj,sel,args...);
        }
    };

    //--------------------------------------------------------------------------

    #include "detail/message_base.hpp"

    template<typename>
    struct message;

    template<typename Result, typename... Args>
    struct message<Result(Args...)> : message_base<Result(Args...)> {
        SEL const sel = nullptr;

        message(selector sel)
        : sel(sel) {}

        Result operator ()(object obj, Args... args) const {
            return send((objc_object*)obj,sel,args...);
        }

        Result operator ()(super sup, Args... args) const {
            return send((objc_super&)(sup),sel,args...);
        }

        template<typename Self>
        Result operator ()(Self* obj, Args... args) const {
            return send((objc_object*)obj,sel,args...);
        }

        using message_base<Result(Args...)>::send;
    };

    //--------------------------------------------------------------------------

    inline
    id alloc(classid cls) {
        message<id()> alloc { "alloc" };
        message<id()> init { "init" };
        return init(alloc(cls));
    }

    template<typename... Args>
    id alloc(classid cls, selector sel, Args... args) {
        message<id()> alloc { "alloc" };
        message<id(Args...)> init { sel };
        return init(alloc(cls),args...);
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

            classid cls {"NSObject"};

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
        isKindOfClass(classid cls) const {
            return api.isKindOfClass(this,cls);
        }

        void
        release() { api.release(this); }

        NSObject*
        retain() { return api.retain(this); }

    };

    NSObject::API
    NSObject::api {};

    //--------------------------------------------------------------------------

    struct NSAutoreleasePool : NSObject {

        static struct API {

            classid cls {"NSAutoreleasePool"};

        } api;

        static
        NSAutoreleasePool*
        alloc() { return NSObject::alloc<NSAutoreleasePool>(); }

        NSAutoreleasePool*
        init() { return NSObject::init<NSAutoreleasePool>(); }

    };

    NSAutoreleasePool::API
    NSAutoreleasePool::api {};

    //--------------------------------------------------------------------------

    struct autoreleasepool {
        NSAutoreleasePool* const pool = NSAutoreleasePool::alloc()->init();
        autoreleasepool() = default;
        autoreleasepool(decltype(nullptr)) : pool(nullptr) {}
       ~autoreleasepool() { delete pool; new(this) autoreleasepool(nullptr); }
    };

    //--------------------------------------------------------------------------

    template<typename ObjectType = NSObject>
    struct NSArray;

    template<>
    struct NSArray<NSObject> : NSObject {

        static struct API {

            classid cls {"NSArray"};

            message<NSArray*(NSObject*const*,NSUInteger)>
            arrayWithObjects_count {"arrayWithObjects:count:"};

            message<NSUInteger()>
            count {"count"};

            message<NSObject*(NSUInteger)>
            objectAtIndex {"objectAtIndex:"};

        } api;

        static
        NSArray*
        array(std::initializer_list<NSObject*> objects) {
            return api.arrayWithObjects_count(
                api.cls,objects.begin(),objects.size()
            );
        }

        NSUInteger count() const { return api.count(this); }

        NSObject*
        object(NSUInteger index) const {
            return api.objectAtIndex(this,index);
        }

    };

    template<typename ObjectType>
    struct NSArray : NSArray<NSObject> {

        static
        NSArray*
        array(std::initializer_list<ObjectType*> objects) {
            return (NSArray*)api.arrayWithObjects_count(
                api.cls,objects.begin(),objects.size()
            );
        }

        ObjectType*
        object(NSUInteger index) const {
            return (ObjectType*)api.objectAtIndex(this,index);
        }

    };

    NSArray<NSObject>::API
    NSArray<NSObject>::api {};

    //--------------------------------------------------------------------------

    template<typename KeyType = NSObject, typename ObjectType = NSObject>
    struct NSDictionary;

    template<>
    struct NSDictionary<NSObject,NSObject> : NSObject {

        static struct API {

            classid cls {"NSDictionary"};

            message<NSDictionary*(NSObject*const*,NSObject*const*,NSUInteger)>
            dictionaryWithObjects_forKeys_count
            {"dictionaryWithObjects:forKeys:count:"};

            message<NSUInteger()>
            count {"count"};

            message<NSObject*(NSObject*)>
            objectForKey {"objectForKey:"};

        } api;

        static
        NSDictionary*
        dictionary(
            std::initializer_list<NSObject*> objects,
            std::initializer_list<NSObject*> keys
        ) {
            assert(objects.size() == keys.size());
            return api.dictionaryWithObjects_forKeys_count(
                api.cls,objects.begin(),keys.begin(),
                std::min(objects.size(),keys.size())
            );
        }

        NSUInteger count() const { return api.count(this); }

        NSObject*
        object(NSObject* key) const {
            return api.objectForKey(this,key);
        }

    };

    template<typename KeyType, typename ObjectType>
    struct NSDictionary : NSDictionary<NSObject,NSObject> {

        static
        NSDictionary*
        dictionary(
            std::initializer_list<ObjectType*> objects,
            std::initializer_list<KeyType*>    keys
        ) {
            assert(objects.size() == keys.size());
            return (NSDictionary*)api.dictionaryWithObjects_forKeys_count(
                api.cls,objects.begin(),keys.begin(),
                std::min(objects.size(),keys.size())
            );
        }

        ObjectType*
        object(NSObject* key) const {
            return (ObjectType*)api.objectForKey(this,key);
        }

    };

    NSDictionary<NSObject,NSObject>::API
    NSDictionary<NSObject,NSObject>::api {};

    //--------------------------------------------------------------------------

    struct NSString : NSObject {

        static struct API {

            classid cls {"NSString"};

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

        const char* UTF8String() const { return api.UTF8String(this); }

    };

    NSString::API
    NSString::api {};

    //--------------------------------------------------------------------------

    struct NSError : NSObject {

        using UserInfo = NSDictionary<NSString,NSObject>;

        static struct API {

            classid cls {"NSError"};

            message<NSError*(NSString*,NSInteger,UserInfo*)>
            errorWithDomain_code_userInfo
            {"errorWithDomain:code:userInfo:"};

            message<NSInteger()>
            code {"code"};

            message<NSString*()>
            domain {"domain"};

            message<UserInfo*()>
            userInfo {"userInfo"};

            message<NSString*()>
            localizedDescription {"localizedDescription"};

            message<NSString*()>
            localizedRecoveryOptions {"localizedRecoveryOptions"};

            message<NSString*()>
            localizedRecoverySuggestion {"localizedRecoverySuggestion"};

            message<NSString*()>
            localizedFailureReason {"localizedFailureReason"};

        } api;

        static
        NSError*
        error(NSString* domain, NSInteger code, UserInfo* userInfo) {
            return api.errorWithDomain_code_userInfo(
                api.cls,domain,code,userInfo
            );
        }

        NSInteger code() const { return api.code(this); }

        NSString* domain() const { return api.domain(this); }

        UserInfo* userInfo() const { return api.userInfo(this); }

        NSString* localizedDescription() const {
            return api.localizedDescription(this);
        }

        NSString* localizedRecoveryOptions() const {
            return api.localizedRecoveryOptions(this);
        }

        NSString* localizedRecoverySuggestion() const {
            return api.localizedRecoverySuggestion(this);
        }

        NSString* localizedFailureReason() const {
            return api.localizedFailureReason(this);
        }

    };

    NSError::API
    NSError::api {};

} // namespace OBJC_NAMESPACE

#else

    #error "objc: unsupported platform"

#endif // CXX_OS_APPLE
#include "dll/cxx/pop.h"
