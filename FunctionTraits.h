#pragma once

namespace pk
{
    template <typename F>
    struct function_trait : function_trait<decltype(&F::operator())> {};

    template <typename F, typename Ret, typename... Args>
    struct function_trait<Ret(F::*)(Args...)const>
    {
        typedef Ret return_type;
    };

    template <typename F, typename Ret, typename P>
    struct function_trait<Ret(F::*)(P)const>
    {
        typedef P   parameter_type;
        typedef Ret return_type;
    };
}
