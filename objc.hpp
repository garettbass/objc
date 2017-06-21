#pragma once
#include "detail/enabled.hpp"

#if OBJC_ENABLED

#include <cassert>
#include <memory>
#include <type_traits>
#include <CoreFoundation/CoreFoundation.h> // CFSTR() macro
#include <objc/runtime.h>
#include <objc/message.h>


#ifdef OBJC_PARENT_NAMESPACE
    #define OBJC_NAMESPACE ::OBJC_PARENT_NAMESPACE::objc
#else
    #define OBJC_NAMESPACE ::objc
#endif // OBJC_PARENT_NAMESPACE


#ifdef OBJC_PARENT_NAMESPACE
namespace OBJC_PARENT_NAMESPACE {
#endif // OBJC_PARENT_NAMESPACE
namespace objc {

    //--------------------------------------------------------------------------

    using class_t = ::Class;

    using id_t = ::id;

    using imp_t = ::IMP;

    using ivar_t = ::Ivar;

    using object_t = std::remove_pointer<id_t>::type;

    using protocol_t = ::Protocol;

    using sel_t = ::SEL;

    using super_t = ::objc_super;

    //--------------------------------------------------------------------------

    struct deleter { void operator ()(id_t obj) { CFRelease(obj); } };

    using unique_ptr = std::unique_ptr<object_t,deleter>;

    using shared_ptr = std::shared_ptr<object_t>;

    //--------------------------------------------------------------------------

    struct object {
        id_t const id;

        object(id_t obj) : id(obj) {}

        object(const unique_ptr& obj) : id(obj.get()) {}

        object(const shared_ptr& obj) : id(obj.get()) {}

        explicit operator bool() const { return id != nullptr; }

        explicit operator size_t() const { return size_t(id); }

        operator id_t() const { return id; }

        bool operator ==(const object& o) const { return id == o.id; }
        bool operator !=(const object& o) const { return id != o.id; }
    };

    //--------------------------------------------------------------------------

    struct class_id {
        class_t const cls;

        class_id(class_t cls) : cls(cls) {}

        class_id(const char* name) : cls(objc_getClass(name)) {}

        class_id(id_t obj) : cls(object_getClass(obj)) {}

        class_id(const unique_ptr& obj) : cls(object_getClass(obj.get())) {}

        class_id(const shared_ptr& obj) : cls(object_getClass(obj.get())) {}

        explicit operator bool() const { return cls != nullptr; }

        operator class_t() { return cls; }

        operator object() { return id_t(cls); }

        bool operator ==(const class_id& c) const { return cls == c.cls; }
        bool operator !=(const class_id& c) const { return cls != c.cls; }

        const char* name() const { return cls ? class_getName(cls) : nullptr; }
    };

    //--------------------------------------------------------------------------

    struct protocol {
        protocol_t* const proto = nullptr;

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
            return proto ? protocol_getName(proto) : nullptr;
        }
    };

    //--------------------------------------------------------------------------

    struct selector {
        sel_t const sel;

        selector(sel_t sel) : sel(sel) {}

        selector(const char* name) : sel(sel_getUid(name)) {}

        explicit operator bool() const { return sel != nullptr; }

        operator sel_t() const { return sel; }

        bool operator ==(const selector& s) const { return sel == s.sel; }
        bool operator !=(const selector& s) const { return sel != s.sel; }

        const char* name() const { return sel ? sel_getName(sel) : nullptr; }
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

    inline
    class_id classof(id_t obj) { return { obj }; }

    inline
    class_id classof(const unique_ptr& obj) { return { obj }; }

    inline
    class_id classof(const shared_ptr& obj) { return { obj }; }

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

    #include "detail/caller.hpp"

    template<typename Result, typename... Args>
    Result call(object obj, selector sel, Args&&... args) {
        return caller<Result>::call(
            obj,sel,std::forward<Args&&>(args)...
        );
    }

    template<typename Result, typename... Args>
    Result call(super sup, selector sel, Args&&... args) {
        return caller<Result>::call(
            sup,sel,std::forward<Args&&>(args)...
        );
    }

    //--------------------------------------------------------------------------

    inline
    id alloc(class_id cls) {
        return call<id>(call<id>(cls,"alloc"),"init");
    }

    template<typename... Args>
    id alloc(class_id cls, selector sel, Args&&... args) {
        return call<id>(
            call<id>(cls,"alloc"),sel,
            std::forward<Args&&>(args)...
        );
    }

    //--------------------------------------------------------------------------

    inline
    unique_ptr make_unique(class_id cls) {
        return unique_ptr { alloc(cls) };
    }

    template<typename... Args>
    unique_ptr make_unique(class_id cls, selector sel, Args&&... args) {
        return unique_ptr { alloc(cls,sel,std::forward<Args&&>(args)...) };
    }

    //--------------------------------------------------------------------------

    inline
    shared_ptr make_shared(class_id cls) {
        return shared_ptr { alloc(cls), CFRelease };
    }

    template<typename... Args>
    shared_ptr make_shared(class_id cls, selector sel, Args&&... args) {
        return shared_ptr {
            alloc(cls,sel,std::forward<Args&&>(args)...),
            CFRelease
        };
    }

    //--------------------------------------------------------------------------

    struct autoreleasepool : private unique_ptr {
        autoreleasepool() : unique_ptr(alloc("NSAutoreleasePool")) {}
    };

    //--------------------------------------------------------------------------

    template<typename T>
    struct member_accessor {
        ivar_t const ivar   = nullptr;
        size_t const offset = 0;

        member_accessor(class_id cls, const char* name)
        : ivar(class_getInstanceVariable(cls,name))
        , offset(ivar ? ivar_getOffset(ivar) : 0) {}

        explicit operator bool() const { return ivar != nullptr; }

        T& operator ()(object obj) const {
            assert(ivar);
            return *((T*)(ivar ? (size_t(obj) + offset) : size_t(0)));
        }
    };

    template<typename T>
    T& member(object obj, const char* name) {
        return member_accessor<T>(obj,name)(obj);
    }

    //--------------------------------------------------------------------------

    template<typename Value>
    struct member_definition {
        const char* const name;

        member_definition(const char* name)
        : name(name) {}
    };

    template<typename Value>
    member_definition<Value>
    member(const char* name) {
        return { name };
    }

    //--------------------------------------------------------------------------

    #include "detail/function_traits.hpp"

    struct method_definition {
        sel_t const sel = nullptr;
        imp_t const imp = nullptr;

        method_definition(selector sel, imp_t imp)
        : sel(sel), imp(imp) {}
    };

    inline
    method_definition
    method(selector sel, imp_t imp) {
        return { sel, imp };
    }

    template<typename Callback>
    method_definition
    method(selector sel, Callback&& callback) {
        using argument_0 = argument_type<Callback,0>;
        static_assert(
            std::is_same<argument_0,id>::value,
            "first argument must be of type 'id'"
        );
        using argument_1 = argument_type<Callback,1>;
        static_assert(
            std::is_same<argument_1,SEL>::value,
            "second argument must be of type 'SEL'"
        );
        return { sel, imp_t(function_cast(callback)) };
    }

    //--------------------------------------------------------------------------

    template<typename MethodSignature>
    struct method_implementation;

    template<typename Result, typename... Args>
    struct method_implementation<Result(Args...)> {
        using imp_t = Result(*)(id,sel_t,Args...);

        sel_t const sel = nullptr;
        imp_t const imp = nullptr;

        method_implementation(class_id cls, selector sel)
        : sel(sel), imp(imp_t(class_getMethodImplementation(cls,sel))) {}

        Result operator ()(object obj, Args&&... args) const {
            return imp(obj,sel,std::forward<Args&&>(args)...);
        }
    };

    //--------------------------------------------------------------------------

    template<typename MethodSignature>
    struct method_interface;

    template<typename Result, typename... Args>
    struct method_interface<Result(Args...)> {
        sel_t const sel = nullptr;

        method_interface(selector sel)
        : sel(sel) {}

        Result operator ()(object obj, Args&&... args) const {
            return call<Result>(obj,sel,std::forward<Args&&>(args)...);
        }

        Result operator ()(super super, Args&&... args) const {
            return call<Result>(super,sel,std::forward<Args&&>(args)...);
        }
    };

    //--------------------------------------------------------------------------

    using protocol_definition = protocol;

    //--------------------------------------------------------------------------

    struct class_definition : class_id {
        template<typename... Defs>
        class_definition(const char* name, class_id super, Defs&&... defs)
        : class_id(objc_allocateClassPair(super,name,0)) {
            struct local { static void unpack(...) {} };
            local::unpack(((define(super,defs)),'\0')...);
            objc_registerClassPair(cls);
        }

    private:

        template<typename T>
        void define(class_id super, const member_definition<T>& def) {
            if (not class_addIvar(cls,def.name,sizeof(T),0,"?")) {
                assert(!"failed to add member");
            }
        }

        void define(class_id super, const method_definition& def) {
            if (not class_addMethod(cls,def.sel,def.imp,"@:")) {
                assert(!"failed to add method");
            }
        }

        void define(class_id super, const protocol_definition& def) {
            if (not class_addProtocol(cls,def)) {
                assert(!"failed to add protocol");
            }
        }
    };

    //--------------------------------------------------------------------------

} // namespace objc
#ifdef OBJC_PARENT_NAMESPACE
} // namespace OBJC_PARENT_NAMESPACE
#endif // OBJC_PARENT_NAMESPACE


#endif // OBJC_ENABLED