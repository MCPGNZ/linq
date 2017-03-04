#pragma once
#include <type_traits>
#include <tuple>

namespace pk
{
    #define IS_TRUE_VALUE(T) typename std::enable_if<T>::type

    //////////////////////////////////////////////////////////////////////////
    // [template typename]: Used for checking input types
    //////////////////////////////////////////////////////////////////////////
    #define IS_TRUE_TYPE(T)  typename std::enable_if<T::value>::type

    #define IS_SAME_TYPE(T, U) typename std::enable_if<std::is_same<T, U>::value>::type
    #define IS_DIFF_TYPE(T, U) typename std::enable_if<!std::is_same<T, U>::value>::type

    #define IS_FLOATING_TYPE(T, U) typename std::enable_if<std::is_same<T, U>::value>::type
    #define IS_INTEGRAL_TYPE(T, U) typename std::enable_if<!std::is_same<T, U>::value>::type
    #define IS_ARITHMETIC_TYPE(T, U) typename std::enable_if<!std::is_same<T, U>::value>::type

    //////////////////////////////////////////////////////////////////////////
    // [method parameter]: Used when we want to dispatch methods for T
    //////////////////////////////////////////////////////////////////////////
    #define IS_TRUE(T)       typename std::enable_if<T::value>::type* _sfinae_ = 0

    #define IS_SAME(T, U)    typename std::enable_if<<std::is_same<T, U>::value>::type* _sfinae_ = 0
    #define IS_DIFF(T, U)    typename std::enable_if<<!std::is_same<T, U>::value>::type* _sfinae_ = 0

    #define IS_FLOATING(T)   typename std::enable_if<std::is_floating_point<T>::value>::type* _sfinae_ = 0
    #define IS_INTEGRAL(T)   typename std::enable_if<std::is_integral<T>::value>::type* _sfinae_ = 0
    #define IS_ARITHMETIC(T) typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value>::type* _sfinae_ = 0

    // Lambdas
    template <typename F>
    struct lambda_traits;

    template <typename F, typename Ret, typename... Args>
    struct lambda_traits<Ret(F::*)(Args...) const>
    {
        static const std::size_t parameter_count = sizeof...(Args);
        using return_type = Ret;

        template <std::size_t N>
        using parameter = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };

    // Query function traits
    template<typename F>
    struct function : lambda_traits<decltype(&F::operator())> {};

    // Functions
    template<typename Ret, typename ...Args>
    struct function<Ret(Args...)>
    {
        static const std::size_t parameter_count = sizeof...(Args);
        using return_type = Ret;

        template <std::size_t N>
        using parameter = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };

    // Methods
    template <typename C, typename Ret, typename... Args>
    struct function<Ret(C::*)(Args...)>
    {
        static const std::size_t parameter_count = sizeof...(Args);
        using return_type = Ret;

        template <std::size_t N>
        using parameter = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
}