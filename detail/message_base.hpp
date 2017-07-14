#pragma once

#if OBJC_CPU_ARM
    #define objc_msgSend_fpret objc_msgSend
    #define objc_msgSend_stret objc_msgSend
    #define objc_msgSendSuper_stret objc_msgSendSuper
#endif

template<typename Result, typename... Args>
using send_self_t = Result(*)(id_t,sel_t,Args...);

template<typename Result, typename... Args>
using send_super_t = Result(*)(super_t*,sel_t,Args...);

//------------------------------------------------------------------------------

template<typename>
struct message_float_result;

template<typename Result, typename... Args>
struct message_float_result<Result(Args...)> {
    static Result send(id_t obj, selector sel, Args... args) {
        return send_self_t<Result,Args...>(objc_msgSend_fpret)(
            obj,sel,std::forward<Args>(args)...
        );
    }

    static Result send(super_t* sup, selector sel, Args... args) {
        return send_super_t<Result,Args...>(objc_msgSendSuper)(
            sup,sel,std::forward<Args>(args)...
        );
    }

    static Result send(super_t& sup, selector sel, Args... args) {
        return send_super_t<Result,Args...>(objc_msgSendSuper)(
            &sup,sel,std::forward<Args>(args)...
        );
    }
};

//------------------------------------------------------------------------------

template<typename>
struct message_large_result;

template<typename Result, typename... Args>
struct message_large_result<Result(Args...)> {
    static Result send(id_t obj, selector sel, Args... args) {
        return send_self_t<Result,Args...>(objc_msgSend_stret)(
            obj,sel,std::forward<Args>(args)...
        );
    }

    static Result send(super_t* sup, selector sel, Args... args) {
        return send_super_t<Result,Args...>(objc_msgSendSuper_stret)(
            sup,sel,std::forward<Args>(args)...
        );
    }

    static Result send(super_t& sup, selector sel, Args... args) {
        return send_super_t<Result,Args...>(objc_msgSendSuper_stret)(
            &sup,sel,std::forward<Args>(args)...
        );
    }
};

//------------------------------------------------------------------------------

template<typename>
struct message_small_result;

template<typename Result, typename... Args>
struct message_small_result<Result(Args...)> {
    static Result send(id_t obj, selector sel, Args... args) {
        return send_self_t<Result,Args...>(objc_msgSend)(
            obj,sel,std::forward<Args>(args)...
        );
    }

    static Result send(super_t* sup, selector sel, Args... args) {
        return send_super_t<Result,Args...>(objc_msgSendSuper)(
            sup,sel,std::forward<Args>(args)...
        );
    }

    static Result send(super_t& sup, selector sel, Args... args) {
        return send_super_t<Result,Args...>(objc_msgSendSuper)(
            &sup,sel,std::forward<Args>(args)...
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
