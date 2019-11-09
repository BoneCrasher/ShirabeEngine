//
// Created by dotti on 07.11.19.
//
#include <type_traits>
#incldue <tuple>

#ifndef __SHIRABEDEVELOPMENT_CPP_TOOLS_H__
#define __SHIRABEDEVELOPMENT_CPP_TOOLS_H__


template <class T, class... Ts>
struct is_any : std::bool_constant<(std::is_same_v<T, Ts> || ...)> {};

template <class T, class... Ts>
struct are_same : std::bool_constant<(std::is_same_v<T, Ts> && ...)> {};

template <typename... Args>
struct variadic_type
{ };

template <typename... Args>
struct to_tuple
{
    typedef std::tuple<typename std::decay<Args>::type...> type;
};

template <typename... Args>
struct to_tuple<variadic_type<Args...>>
{
    typedef typename to_tuple<typename std::decay<Args>::type...>::type type;
};

template<typename... T>
using variadic_tuple_t = typename to_tuple<variadic_type<T...>>::type;

namespace test
{
    class Test0 {};
    class Test1 {};

    template <typename... T>
    static void bind(T&&... aTypes)
    {
        if constexpr(std::is_same_v<variadic_tuple_t<T...>
                                  , variadic_tuple_t<Test0, Test1>>)
        {
            // Handling for provided types Test0, Test1
            // std::cout << "Test0, Test1\n";
        }
        else
        {
            // std::cout << "Unknown\n";
        }
    }

    static void test()
    {
        bind(Test0 {}, Test1 {});
    }
}

#endif //__SHIRABEDEVELOPMENT_CPP_TOOLS_H__
