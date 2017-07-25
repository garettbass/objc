#pragma once

template<typename Result, typename... Args>
using send_self_t = Result(*)(objc_object*,SEL,Args...);

template<typename Result, typename... Args>
using send_objc_super = Result(*)(objc_super*,SEL,Args...);

//------------------------------------------------------------------------------

template<typename>
struct message_float_result;

template<typename Result, typename... Args>
struct message_float_result<Result(Args...)> {
    static Result send(objc_object* obj, selector sel, Args... args) {
        return send_self_t<Result,Args...>(objc_msgSend_fpret)(
            obj,sel,args...
        );
    }

    static Result send(objc_super* sup, selector sel, Args... args) {
        return send_objc_super<Result,Args...>(objc_msgSendSuper)(
            sup,sel,args...
        );
    }

    static Result send(objc_super& sup, selector sel, Args... args) {
        return send_objc_super<Result,Args...>(objc_msgSendSuper)(
            &sup,sel,args...
        );
    }
};

//------------------------------------------------------------------------------

template<typename>
struct message_large_result;

template<typename Result, typename... Args>
struct message_large_result<Result(Args...)> {
    static Result send(objc_object* obj, selector sel, Args... args) {
        return send_self_t<Result,Args...>(objc_msgSend_stret)(
            obj,sel,args...
        );
    }

    static Result send(objc_super* sup, selector sel, Args... args) {
        return send_objc_super<Result,Args...>(objc_msgSendSuper_stret)(
            sup,sel,args...
        );
    }

    static Result send(objc_super& sup, selector sel, Args... args) {
        return send_objc_super<Result,Args...>(objc_msgSendSuper_stret)(
            &sup,sel,args...
        );
    }
};

//------------------------------------------------------------------------------

template<typename>
struct message_small_result;

template<typename Result, typename... Args>
struct message_small_result<Result(Args...)> {
    static Result send(objc_object* obj, selector sel, Args... args) {
        return send_self_t<Result,Args...>(objc_msgSend)(
            obj,sel,args...
        );
    }

    static Result send(objc_super* sup, selector sel, Args... args) {
        return send_objc_super<Result,Args...>(objc_msgSendSuper)(
            sup,sel,args...
        );
    }

    static Result send(objc_super& sup, selector sel, Args... args) {
        return send_objc_super<Result,Args...>(objc_msgSendSuper)(
            &sup,sel,args...
        );
    }
};

//------------------------------------------------------------------------------

template<typename T>
struct message_result_size {
    enum : size_t { value = sizeof(T) };
};

template<>
struct message_result_size<void> {
    enum : size_t { value = 0 };
};

//------------------------------------------------------------------------------

template <typename>
struct message_base;

template<typename Result, typename... Args>
struct message_base<Result(Args...)> : std::conditional<
    std::is_floating_point<Result>::value,
    message_float_result<Result(Args...)>,
    typename std::conditional<
        (message_result_size<Result>::value <= sizeof(long double)),
        message_small_result<Result(Args...)>,
        message_large_result<Result(Args...)>
    >::type
>::type {};
