#include "traits.h"

namespace pk
{
    namespace internals
    {
        #pragma region Types
        template <int N> struct empty_proxy {};
        template <typename V, int N> struct value_proxy
        {
            const V value;
        };
        template <typename F, int N> struct method_proxy
        {
            const F& method;
        };
        #pragma endregion

        template <typename F> using where_proxy = method_proxy<F, 0>;
        template <typename F> using select_proxy = method_proxy<F, 1>;
        template <typename F> using foreach_proxy = method_proxy<F, 2>;
        template <typename F> using any_proxy = method_proxy<F, 3>;
        template <typename F> using all_proxy = method_proxy<F, 4>;
        template <typename F> using contains_proxy = value_proxy<F, 5>;
        using count_proxy = empty_proxy<6>;
        using min_proxy = empty_proxy<7>;
        using max_proxy = empty_proxy<8>;
        using sum_proxy = empty_proxy<9>;
    }

    #pragma region Proxies
    template <typename F> decltype(auto) where(F function)
    {
        return internals::where_proxy<F>{function};
    }
    template <typename F> decltype(auto) select(F function)
    {
        return internals::select_proxy<F>{function};
    }
    template <typename F> decltype(auto) foreach(F function)
    {
        return internals::foreach_proxy<F>{function};
    }
    template <typename F> decltype(auto) any(F function)
    {
        return internals::any_proxy<F>{function};
    }
    template <typename F> decltype(auto) all(F function)
    {
        return internals::all_proxy<F>{function};
    }
    template <typename V> decltype(auto) contains(V value)
    {
        return internals::contains_proxy<V>{value};
    }
    decltype(auto) count()
    {
        return internals::count_proxy{};
    };
    decltype(auto) min()
    {
        return internals::min_proxy{};
    };
    decltype(auto) max()
    {
        return internals::max_proxy{};
    };
    decltype(auto) sum()
    {
        return internals::sum_proxy{};
    }
    #pragma endregion

    #pragma region Transformations
    //[where]
    template < template <typename T, typename... A> class Collection, typename T, typename F, typename... A,
        typename = IS_TRUE_VALUE(function<F>::parameter_count == 1),
        typename = IS_SAME_TYPE(function<F>::return_type, bool),
        typename = IS_SAME_TYPE(function<F>::parameter<0>, const T&)
    >
        decltype(auto) operator >> (const Collection<T, A...>& collection, const internals::where_proxy<F>& where)
    {
        Collection<T, A...> result;

        const auto size = collection.size();
        for(std::remove_cv_t<decltype(size)> i = 0; i < size; i++)
        {
            const T& value = collection[i];
            if(where.method(value))
            {
                result.emplace_back(value);
            }
        }

        return result;
    }

    //[select]
    template < template <typename T, typename... A> class Collection, typename T, typename F, typename... A,
        typename = IS_TRUE_VALUE(function<F>::parameter_count == 1),
        typename = IS_SAME_TYPE(function<F>::parameter<0>, const T&),
        typename = IS_DIFF_TYPE(function<F>::return_type, void)
    >
        decltype(auto) operator >> (const Collection<T, A...>& collection, const internals::select_proxy<F>& select)
    {
        const auto size = collection.size();

        Collection<function<F>::return_type, A...> result(size);
        for(std::remove_cv_t<decltype(size)> i = 0; i < size; i++)
        {
            result[i] = select.method(collection[i]);
        }
        return result;
    }

    //[foreach]
    template < template <typename T, typename... A> class Collection, typename T, typename F, typename... A,
        typename = IS_TRUE_VALUE(function<F>::parameter_count == 1),
        typename = IS_SAME_TYPE(function<F>::parameter<0>, T&),
        typename = IS_SAME_TYPE(function<F>::return_type, void)
    >
        decltype(auto) operator >> (Collection<T, A...>& collection, const internals::foreach_proxy<F>& foreach)
    {
        for(auto& value : collection)
        {
            foreach.method(value);
        }
        return collection;
    }
    #pragma endregion

    #pragma region Queries
    //[any]
    template < template <typename T, typename... A> class Collection, typename T, typename F, typename... A,
        typename = IS_TRUE_VALUE(function<F>::parameter_count == 1),
        typename = IS_SAME_TYPE(function<F>::parameter<0>, const T&),
        typename = IS_SAME_TYPE(function<F>::return_type, bool)
    >
        decltype(auto) operator >> (Collection<T, A...>& collection, const internals::any_proxy<F>& any)
    {
        for(auto& value : collection)
        {
            if(any.method(value))
            {
                return true;
            }
        }
        return false;
    }

    //[all]
    template < template <typename T, typename... A> class Collection, typename T, typename F, typename... A,
        typename = IS_TRUE_VALUE(function<F>::parameter_count == 1),
        typename = IS_SAME_TYPE(function<F>::parameter<0>, const T&),
        typename = IS_SAME_TYPE(function<F>::return_type, bool)
    >
        decltype(auto) operator >> (Collection<T, A...>& collection, const internals::all_proxy<F>& all)
    {
        for(auto& value : collection)
        {
            if(!all.method(value))
            {
                return false;
            }
        }
        return true;
    }

    //[contains]
    template < template <typename T, typename... A> class Collection, typename T, typename V, typename... A,
        typename = IS_SAME_TYPE(V, T)
    >
        decltype(auto) operator >> (Collection<T, A...>& collection, const internals::contains_proxy<V>& contains)
    {
        for(auto& value : collection)
        {
            if(value == contains.value)
            {
                return true;
            }
        }
        return false;
    }
    #pragma endregion

    #pragma region Arithmetic
    //[count]
    template < template <typename T, typename... A> class Collection, typename T, typename... A>
    decltype(auto) operator >> (const Collection<T, A...>& collection, const internals::count_proxy&)
    {
        return collection.size();
    }

    //[min]
    template < template <typename T, typename... A> class Collection, typename T, typename... A>
    decltype(auto) operator >> (const Collection<T, A...>& collection, const internals::min_proxy&)
    {
        const auto size = collection.size();

        std::remove_cv_t<decltype(size)> index = 0;
        for(std::remove_cv_t<decltype(size)> i = 1; i < size; i++)
        {
            const T& value = collection[i];
            if(value < collection[index])
            {
                index = i;
            }
        }

        return collection[index];
    }

    //[max]
    template < template <typename T, typename... A> class Collection, typename T, typename... A>
    decltype(auto) operator >> (const Collection<T, A...>& collection, const internals::max_proxy&)
    {
        const auto size = collection.size();

        std::remove_cv_t<decltype(size)> index = 0;
        for(std::remove_cv_t<decltype(size)> i = 1; i < size; i++)
        {
            const T& value = collection[i];
            if(value > collection[index])
            {
                index = i;
            }
        }

        return collection[index];
    }

    //[sum]
    template < template <typename T, typename... A> class Collection, typename T, typename... A>
    decltype(auto) operator >> (const Collection<T, A...>& collection, const internals::sum_proxy&)
    {
        T result = 0;
        for(const auto& value : collection)
        {
            result += value;
        }

        return result;
    }
    #pragma endregion
}