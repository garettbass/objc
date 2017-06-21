#pragma once

template<typename Result>
struct caller;

template<typename Result, typename... Args>
using call_self_t = Result(*)(id_t,sel_t,Args...);

template<typename Result, typename... Args>
using call_super_t = Result(*)(super_t*,sel_t,Args...);

template<typename Result>
class float_result_caller {
    friend struct caller<Result>;

    template<typename... Args>
    static
    Result call(id_t self, sel_t method, Args&&... args) {
        return call_self_t<Result,Args...>(objc_msgSend_fpret)(
            self,method,std::forward<Args&&>(args)...
        );
    }

    template<typename... Args>
    static
    Result call(super_t* super, sel_t method, Args&&... args) {
        return call_super_t<Result,Args...>(objc_msgSendSuper)(
            super,method,std::forward<Args&&>(args)...
        );
    }
};

template<typename Result>
class large_result_caller {
    friend struct caller<Result>;

    template<typename... Args>
    static
    Result call(id_t self, sel_t method, Args&&... args) {
        return call_self_t<Result,Args...>(objc_msgSend_stret)(
            self,method,std::forward<Args&&>(args)...
        );
    }

    template<typename... Args>
    static
    Result call(super_t* super, sel_t method, Args&&... args) {
        return call_super_t<Result,Args...>(objc_msgSendSuper_stret)(
            super,method,std::forward<Args&&>(args)...
        );
    }
};

template<typename Result>
class small_result_caller {
    friend struct caller<Result>;

    template<typename... Args>
    static
    Result call(id_t self, sel_t method, Args&&... args) {
        return call_self_t<Result,Args...>(objc_msgSend)(
            self,method,std::forward<Args&&>(args)...
        );
    }

    template<typename... Args>
    static
    Result call(super_t* super, sel_t method, Args&&... args) {
        return call_super_t<Result,Args...>(objc_msgSendSuper)(
            super,method,std::forward<Args&&>(args)...
        );
    }
};

template <typename Result>
using value_result_caller =
    typename std::conditional<
        std::is_floating_point<Result>::value,
        float_result_caller<Result>,
        typename std::conditional<
            (sizeof(Result) <= sizeof(long double)),
            small_result_caller<Result>,
            large_result_caller<Result>
        >::type
    >::type;

using void_result_caller = small_result_caller<void>;

template<typename Result>
struct caller : value_result_caller<Result> {
    using value_result_caller<Result>::call;
};

template<>
struct caller<void> : void_result_caller {
    using void_result_caller::call;
};