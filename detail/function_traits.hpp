#pragma once

namespace OBJC_NAMESPACE {

    template<typename Struct>
    struct function_traits
    : function_traits<decltype(&Struct::operator())>
    {};

    template<typename Struct, typename Result, typename... Args>
    struct function_traits<Result(Struct::*)(Args...)>
    : function_traits<Result(*)(Args...)> {};

    template<typename Struct, typename Result, typename... Args>
    struct function_traits<Result(Struct::*)(Args...) const>
    : function_traits<Result(*)(Args...)> {};

    template <typename Result, typename... Args>
    struct function_traits<Result(Args...)>
    : function_traits<Result(*)(Args...)> {};

    template <typename Result, typename... Args>
    struct function_traits<Result(*)(Args...)> {
        enum { argument_count = sizeof...(Args) };

        using argument_tuple = std::tuple<Args...>;

        template<unsigned N>
        using argument_type =
            typename std::tuple_element<N,argument_tuple>::type;

        using function_type = Result(*)(Args...);

        using result_type = Result;
    };

    template<typename Function, unsigned N>
    using argument_type =
        typename function_traits<Function>::template argument_type<N>;

    template<typename Function>
    using function_type = typename function_traits<Function>::function_type;

    template<typename Function>
    using result_type = typename function_traits<Function>::result_type;

    template<typename Function>
    static
    function_type<Function>
    function_cast(Function& function) {
        return function_type<Function>(function);
    }

} // namespace OBJC_NAMESPACE