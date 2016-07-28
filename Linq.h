
#include <type_traits>
#include "Effigia\Utility\Traits.h"

namespace pk
{
    #pragma region Helper Macros
    #define METHOD(X)   \
     __forceinline Internals::##X##_method X()  \
     {                                          \
         return Internals::##X##_method{};      \
     }

    #define METHOD_F(X)     \
     template <typename F>  \
     __forceinline Internals::##X##_method<F> X(F function) \
     {                                                      \
         return Internals::##X##_method<F>{function};       \
     }

    #define METHOD_V(X)     \
     template <typename T>  \
     __forceinline Internals::##X##_method<T> X(T value) \
     {                                                   \
         return Internals::##X##_method<T>{value};       \
     }

    #define METHOD_2V(X)     \
     template <typename T, typename U>  \
     __forceinline Internals::##X##_method<T, U> X(T t, U u)  \
     {                                                     \
         return Internals::##X##_method<T, U>{t, u};       \
     }

     #pragma endregion Helper Macros
    #pragma region Helper Methods

    #pragma endregion Helper Methods

    #pragma region Internals
    namespace Internals
    {
        template <typename F> struct where_method
        {
            const F& method;
        };
        template <typename F> struct select_method
        {
            const F& method;
        };
        template <typename F> struct any_method
        {
            const F& method;
        };
        template <typename F> struct all_method
        {
            const F& method;
        };
        template <typename F> struct foreach_method
        {
            const F& method;
        };

        template <typename T> struct contains_method 
        {
            const T& value;
        };

        struct sum_method {};
        struct min_method {};
        struct max_method {};

        struct count_method {};

        template <typename T> struct skip_method
        {
            const int value;
        };
        template <typename T> struct take_method
        {
            const int value;
        };
        template <typename T, typename U> struct range_method
        {
            const int start;
            const int end;
        };
    }
    #pragma endregion Internals

    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    struct range_evaluation
    {
        #pragma region Variables
        const Collection<T>& collection;

        int start;
        int end;
        #pragma endregion Variables

        #pragma region Constructors
        range_evaluation(const Collection<T>& c) : collection{c}
        {
            start = 0;
            end = collection.size();
        }
        range_evaluation(const range_evaluation& r) :
            collection{r.collection},
            start{r.start},
            end{r.end}
        {}
        #pragma endregion Constructors

        #pragma region Conversion
        operator Collection<T>() const
        {
            return evaluate();
        }

        //[memcpy]
        template <typename Q = T>
        Collection<T> evaluate(ENABLE_PARAM(std::is_trivially_copyable<Q>::value)) const
        {
            const int count = end - start;

            Collection<T> result(count);
            memcpy(result.data(), collection.data() + start, sizeof(T) * count);

            return result;
        }

        //[copy construction]
        template <typename Q = T>
        Collection<T> evaluate(ENABLE_PARAM(!std::is_trivially_copyable<Q>::value)) const
        {
            Collection<T> result(end - start);
            for(int i = start; i < end; ++i)
            {
                result[i - start] = collection[i];
            }

            return result;
        }
        #pragma endregion Conversion
    };

    #pragma region Transformations
    //[where]
    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T, typename F,
        typename = std::enable_if_t<std::is_same<function_trait<F>::parameter_type, T>::value>>
    Collection<T> operator >> (const Collection<T>& collection, const Internals::where_method<F>& where)
    {
        Collection<T> result;

        const int size = static_cast<int>(collection.size());
        for(int i = 0; i < size; ++i)
        {
            const T& value = collection[i];
            if(where.method(value))
            {
                result.emplace_back(value);
            }
        }

        return std::move(result);
    }

    //[select]
    template <template <typename T, typename A = std::allocator<T>> class Collection, typename T, typename F,
        typename = std::enable_if_t<std::is_same<function_trait<F>::parameter_type, T>::value>>
    decltype(auto) operator >> (const Collection<T>& collection, const Internals::select_method<F>& select) 
    {
        const int size = static_cast<int>(collection.size());

        std::vector<function_trait<F>::return_type> result(size);
        for(int i = 0; i < size; ++i)
        {
            const T& value = collection[i];
            result[i] = select.method(value);
        }

        return result;
    }

    //[foreach]
    template <template <typename T, typename A = std::allocator<T>> class Collection, typename T, typename F, 
              typename = std::enable_if_t<std::is_reference<function_trait<F>::parameter_type>::value>,
              typename = std::enable_if_t<std::is_same<std::remove_reference_t<function_trait<F>::parameter_type>, T>::value>>
    Collection<T>& operator >> (Collection<T>& collection, const Internals::foreach_method<F>& foreach) 
    {
        const int size = static_cast<int>(collection.size());
        for(int i = 0; i < size; ++i)
        {
            T& value = collection[i];
            foreach.method(value);
        }

        return collection;
    }
    #pragma endregion Transformations

    #pragma region Queries
    //[any]
    template <template <typename T, typename A = std::allocator<T>> class Collection, typename T, typename F,
        typename = std::enable_if_t<std::is_same<function_trait<F>::parameter_type, T>::value>>
        bool operator >> (const Collection<T>& collection, const Internals::any_method<F>& any)
    {
        const int size = static_cast<int>(collection.size());
        for(int i = 0; i < size; ++i)
        {
            const T& value = collection[i];
            if(any.method(value))
            {
                return true;
            }
        }

        return false;
    }

    //[all]
    template <template <typename T, typename A = std::allocator<T>> class Collection, typename T, typename F,
        typename = std::enable_if_t<std::is_same<function_trait<F>::parameter_type, T>::value>>
        bool operator >> (const Collection<T>& collection, const Internals::all_method<F>& all)
    {
        const int size = static_cast<int>(collection.size());
        for(int i = 0; i < size; ++i)
        {
            const T& value = collection[i];
            if(!all.method(value))
            {
                return false;
            }
        }

        return true;
    }

    //[contains]
    template <template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    bool operator >> (Collection<T>& collection, const Internals::contains_method<T>& contains)
    {
        const int size = static_cast<int>(collection.size());
        for(int i = 0; i < size; ++i)
        {
            const T& value = collection[i];
            if(value == contains.value)
            {
                return true;
            }
        }

        return false;
    }
    #pragma endregion Queries

    #pragma region Arithmetic
    //[sum]
    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    T operator >> (const Collection<T>& collection, const Internals::sum_method&)
    {
        T result = static_cast<T>(0);

        const int size = static_cast<int>(collection.size());
        for(int i = 0; i < size; ++i)
        {
            const T& value = collection[i];
            result += value;
        }

        return result;
    }

    //[min]
    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    T operator >> (const Collection<T>& collection, const Internals::min_method&)
    {
        T result = collection[0];

        const int size = static_cast<int>(collection.size());
        for(int i = 1; i < size; ++i)
        {
            const T& value = collection[i];
            if(result > value)
            {
                result = value;
            }
        }

        return result;
    }

    //[max]
    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    T operator >> (const Collection<T>& collection, const Internals::max_method&)
    {
        T result = collection[0];

        const int size = static_cast<int>(collection.size());
        for(int i = 1; i < size; ++i)
        {
            const T& value = collection[i];
            if(result < value)
            {
                result = value;
            }
        }

        return result;
    }

    //[count]
    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    int operator >> (const Collection<T>& collection, const Internals::count_method&)
    {
        return static_cast<int>(collection.size());
    }
    #pragma endregion Arithmetic

    #pragma region Ranges
    //[take]
    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    range_evaluation<Collection, T> operator >> (const Collection<T>& collection, const Internals::take_method<int>& info)
    {
        range_evaluation<Collection, T> result{collection};
        result.end = result.start + info.value;

        return result;
    }

    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    range_evaluation<Collection, T>& operator >> (range_evaluation<Collection, T>& range, const Internals::take_method<int>& info)
    {
        range.end = range.start + info.value;
        return range;
    }

    //[skip]
    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    range_evaluation<Collection, T> operator >> (const Collection<T>& collection, const Internals::skip_method<int>& info)
    {
        range_evaluation<Collection, T> result{collection};
        result.start += info.value;

        return result;
    }

    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    range_evaluation<Collection, T>& operator >> (range_evaluation<Collection, T>& range, const Internals::skip_method<int>& info)
    {
        range.start += info.value;
        return range;
    }

    //[range]
    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    range_evaluation<Collection, T> operator >> (const Collection<T>& collection, const Internals::range_method<int, int>& info)
    {
        range_evaluation<Collection, T> result{collection};
        result.start = info.start;
        result.end   = info.end;

        return result;
    }

    template < template <typename T, typename A = std::allocator<T>> class Collection, typename T>
    range_evaluation<Collection, T>& operator >> (range_evaluation<Collection, T>& range, const Internals::range_method<int, int>& info)
    {
        range.start = info.start;
        range.end   = info.end;
        return range;
    }

    #pragma region Functions
    METHOD(sum);
    METHOD(min);
    METHOD(max);
    METHOD(count);

    METHOD_V(contains);

    METHOD_V(skip);
    METHOD_V(take);
    METHOD_2V(range);

    METHOD_F(where);
    METHOD_F(select);
    METHOD_F(any);
    METHOD_F(foreach);
    METHOD_F(all);
    #pragma endregion Functions
}