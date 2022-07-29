// Tuple type with flat memory layout and linear implementation.
// Copyright (C) 2022 Natalie Wiggins
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#ifndef DEMONIA_SRC_TUPLE_HH
#define DEMONIA_SRC_TUPLE_HH

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

#ifndef __has_feature
#define __has_feature(x) 0
#endif

namespace demonia
{

template<typename... Ts_>
class Tuple;

namespace detail
{

// TupleLeaf

template<size_t I_, typename ValueType_,
         bool IsEmpty_ = std::is_empty<ValueType_>::value
#if __has_feature(is_final)
            && !__is_final(ValueType_)
#endif
        >
class TupleLeaf;

template<size_t I, typename ValueType, bool IsEmpty>
inline void swap(TupleLeaf<I, ValueType, IsEmpty>& a,
                 TupleLeaf<I, ValueType, IsEmpty>& b)
{
    std::swap(a.get(), b.get());
}

template<size_t I_, typename ValueType_, bool IsEmpty_>
class TupleLeaf
{
public:
    TupleLeaf()
            : value_()
    {
        static_assert(!std::is_reference<ValueType_>::value,
                      "cannot default construct a reference element in a \
                       tuple");
    }

    TupleLeaf(const TupleLeaf& t)
            : value_(t.get())
    {
        static_assert(!std::is_rvalue_reference<ValueType_>::value,
                      "cannot copy a tuple with an rvalue reference member");
    }

    template<typename T,
             typename = typename std::enable_if<
                    std::is_constructible<ValueType_, T>::value>::type>
    explicit TupleLeaf(T&& t)
            : value_(std::forward<T>(t))
    {
        static_assert(!std::is_reference<ValueType_>::value
                      || (std::is_lvalue_reference<ValueType_>::value
                          && (std::is_lvalue_reference<T>::value
                              || std::is_same<
                                    typename std::remove_reference<T>::type,
                                    std::reference_wrapper<
                                        typename std::remove_reference<
                                            ValueType_>::type>>::value))
                      || (std::is_rvalue_reference<ValueType_>::value
                          && !std::is_lvalue_reference<T>::value),
                      "cannot construct a reference element in a tuple with an \
                       rvalue");
    }

    template<typename T>
    explicit TupleLeaf(const TupleLeaf<I_, T>& t)
            : value_(t.get())
    {
    }

    template<typename T>
    TupleLeaf& operator=(T&& t)
    {
        value_ = std::forward<T>(t);
        return *this;
    }

    int swap(TupleLeaf& t)
    {
        swap(*this, t);
        return 0;
    }

    ValueType_& get()
    {
        return value_;
    }

    const ValueType_& get() const
    {
        return value_;
    }

private:
    ValueType_ value_;

    TupleLeaf& operator=(const TupleLeaf&) = delete;
};

// Empty
template<size_t I_, typename ValueType_>
class TupleLeaf<I_, ValueType_, true> : private ValueType_
{
public:
    TupleLeaf()
    {
    }

    TupleLeaf(const TupleLeaf& t)
            : ValueType_(t.get())
    {
        static_assert(!std::is_rvalue_reference<ValueType_>::value,
                      "cannot copy a tuple with an rvalue reference member");
    }

    template<typename T,
             typename = typename std::enable_if<
                    std::is_constructible<ValueType_, T>::value>::type>
    explicit TupleLeaf(T&& t)
            : ValueType_(std::forward<T>(t))
    {
    }

    template<typename T>
    explicit TupleLeaf(const TupleLeaf<I_, T>& t)
            : ValueType_(t.get())
    {
    }

    template<typename T>
    TupleLeaf& operator=(T&& t)
    {
        ValueType_::operator=(std::forward<T>(t));
        return *this;
    }

    int swap(TupleLeaf& t)
    {
        swap(*this, t);
        return 0;
    }

    ValueType_& get()
    {
        return static_cast<ValueType_&>(*this);
    }

    const ValueType_& get() const
    {
        return static_cast<const ValueType_&>(*this);
    }

private:
    TupleLeaf& operator=(const TupleLeaf&) = delete;
};

// TupleIndexes

template<size_t... Is_>
struct TupleIndexes
{
};

template<size_t Start_, typename TupleIndexes_, size_t End_>
struct MakeTupleIndexesImpl;

template<size_t Start_, size_t... Indexes_, size_t End_>
struct MakeTupleIndexesImpl<Start_, TupleIndexes<Indexes_...>, End_>
{
    typedef typename MakeTupleIndexesImpl<Start_ + 1,
                TupleIndexes<Indexes_..., Start_>, End_>::type type;
};

template<size_t End_, size_t... Indexes_>
struct MakeTupleIndexesImpl<End_, TupleIndexes<Indexes_...>, End_>
{
    typedef TupleIndexes<Indexes_...> type;
};

template<size_t End_, size_t Start_ = 0>
struct MakeTupleIndexes
{
    static_assert(Start_ <= End_, "cannot make tuple indexes for range with a \
                                   start index greater than its end index");
    typedef typename MakeTupleIndexesImpl<Start_, TupleIndexes<>, End_>::type
            type;
};

// TupleTypes

template<typename... Ts_>
struct TupleTypes
{
};

// TupleSize

template<typename T_>
struct TupleSize : public std::tuple_size<T_>
{
};

template<typename T_>
struct TupleSize<const T_> : public TupleSize<T_>
{
};

template<typename T_>
struct TupleSize<volatile T_> : public TupleSize<T_>
{
};

template<typename T_>
struct TupleSize<const volatile T_> : public TupleSize<T_>
{
};

template<typename... Ts_>
struct TupleSize<TupleTypes<Ts_...>>
        : public std::integral_constant<size_t, sizeof...(Ts_)>
{
};

template<typename... Ts_>
struct TupleSize<Tuple<Ts_...>>
        : public std::integral_constant<size_t, sizeof...(Ts_)>
{
};

// TupleElement

template<size_t I_, typename T_>
class TupleElement : public std::tuple_element<I_, T_>
{
};

template<size_t I_>
class TupleElement<I_, TupleTypes<>>
{
public:
    static_assert(I_ != I_, "tuple_element index out of range");
};

template<typename H_, typename... Ts_>
class TupleElement<0, TupleTypes<H_, Ts_...>>
{
public:
    typedef H_ type;
};

template<size_t I_, typename H_, typename... Ts_>
class TupleElement<I_, TupleTypes<H_, Ts_...>>
{
public:
    typedef typename TupleElement<I_ - 1, TupleTypes<Ts_...>>::type type;
};

template<size_t I_, typename... Ts_>
class TupleElement<I_, Tuple<Ts_...>>
{
public:
    typedef typename TupleElement<I_, TupleTypes<Ts_...>>::type type;
};

template<size_t I_, typename... Ts_>
class TupleElement<I_, const Tuple<Ts_...>>
{
public:
    typedef typename std::add_const<typename TupleElement<I_, TupleTypes<Ts_...>
                >::type>::type type;
};

template<size_t I_, typename... Ts_>
class TupleElement<I_, volatile Tuple<Ts_...>>
{
public:
    typedef typename std::add_volatile<typename TupleElement<I_,
                TupleTypes<Ts_...>>::type>::type type;
};

template<size_t I_, typename... Ts_>
class TupleElement<I_, const volatile Tuple<Ts_...>>
{
public:
    typedef typename std::add_cv<typename TupleElement<I_, TupleTypes<Ts_...>
                >::type>::type type;
};

// MakeTupleTypes

template<typename TupleTypes_, typename Tuple_, size_t Start_, size_t End_>
struct MakeTupleTypesImpl;

template<typename... Types_, typename Tuple_, size_t Start_, size_t End_>
struct MakeTupleTypesImpl<TupleTypes<Types_...>, Tuple_, Start_, End_>
{
    typedef typename std::remove_reference<Tuple_>::type TupleType;
    typedef typename MakeTupleTypesImpl<TupleTypes<Types_...,
                typename std::conditional<std::is_lvalue_reference<Tuple_>::value,
                    // append ref if Tuple_ is ref
                    typename TupleElement<Start_, TupleType>::type&,
                    // append non-ref otherwise
                    typename TupleElement<Start_, TupleType>::type>::type>,
                Tuple_, Start_ + 1, End_>::type type;
};

template<typename... Types_, typename Tuple_, size_t End_>
struct MakeTupleTypesImpl<TupleTypes<Types_...>, Tuple_, End_, End_>
{
    typedef TupleTypes<Types_...> type;
};

template<typename Tuple_,
         size_t End_ = TupleSize<typename std::remove_reference<Tuple_>::type
                                >::value,
         size_t Start_ = 0>
struct MakeTupleTypes
{
    static_assert(Start_ <= End_, "cannot make tuple types for range with a \
                                   start index greater than its end index");
    typedef typename MakeTupleTypesImpl<TupleTypes<>, Tuple_, Start_, End_
                                       >::type type;
};

// TupleImpl

template<typename... Ts>
void swallow(Ts&&...)
{
}

template<typename TupleIndexes_, typename... Ts_>
struct TupleImpl;

template<size_t... Indexes_, typename... Ts_>
struct TupleImpl<TupleIndexes<Indexes_...>, Ts_...>
        : public TupleLeaf<Indexes_, Ts_>...
{
    template<size_t... FirstIndexes, typename... FirstTypes,
             size_t... LastIndexes, typename... LastTypes,
             typename... ValueTypes>
    explicit TupleImpl(TupleIndexes<FirstIndexes...>, TupleTypes<FirstTypes...>,
                       TupleIndexes<LastIndexes...>, TupleTypes<LastTypes...>,
                       ValueTypes&&... values)
            : TupleLeaf<FirstIndexes, FirstTypes>(
                    std::forward<ValueTypes>(values))...,
              TupleLeaf<LastIndexes, LastTypes>()...
    {
    }

    template<typename OtherTuple>
    TupleImpl(OtherTuple&& t)
            : TupleLeaf<Indexes_, Ts_>(std::forward<typename TupleElement<Indexes_,
                        typename MakeTupleTypes<OtherTuple>::type>::type>(
                    std::get<Indexes_>(t)))...
    {
    }

    template<typename OtherTuple>
    TupleImpl& operator=(OtherTuple&& t)
    {
        swallow(TupleLeaf<Indexes_, Ts_>::operator=(std::forward<
                    typename TupleElement<Indexes_,
                        typename MakeTupleTypes<OtherTuple>::type>::type>(
                    std::get<Indexes_>(t)))...);
        return *this;
    }

    TupleImpl& operator=(const TupleImpl& t)
    {
        swallow(TupleLeaf<Indexes_, Ts_>::operator=(static_cast<
                    const TupleLeaf<Indexes_, Ts_>&>(t).get())...);
        return *this;
    }

    void swap(TupleImpl& t)
    {
        swallow(TupleLeaf<Indexes_, Ts_>::swap(
                    static_cast<TupleLeaf<Indexes_, Ts_>&>(t))...);
    }
};

// TupleLike

template<typename T_>
struct TupleLike : public std::false_type
{
};

template<typename T_>
struct TupleLike<const T_> : public TupleLike<T_>
{
};

template<typename T_>
struct TupleLike<volatile T_> : public TupleLike<T_>
{
};

template<typename T_>
struct TupleLike<const volatile T_> : public TupleLike<T_>
{
};

template<typename... Ts_>
struct TupleLike<Tuple<Ts_...>> : public std::true_type
{
};

template<typename... Ts_>
struct TupleLike<TupleTypes<Ts_...>> : public std::true_type
{
};

template<typename... Ts_>
struct TupleLike<std::tuple<Ts_...>> : public std::true_type
{
};

template<typename First_, typename Second_>
struct TupleLike<std::pair<First_, Second_>> : public std::true_type
{
};

template<typename T_, size_t N_>
struct TupleLike<std::array<T_, N_>> : public std::true_type
{
};

// TupleConvertible

template<bool IsSameSize_, typename From_, typename To_>
struct TupleConvertibleImpl : public std::false_type
{
};

template<typename FromFirst_, typename... FromRest_,
         typename ToFirst_, typename... ToRest_>
struct TupleConvertibleImpl<true, TupleTypes<FromFirst_, FromRest_...>,
            TupleTypes<ToFirst_, ToRest_...>>
        : public std::integral_constant<bool,
            std::is_convertible<FromFirst_, ToFirst_>::value
            && TupleConvertibleImpl<true, TupleTypes<FromRest_...>,
                TupleTypes<ToRest_...>>::value>
{
};

template<>
struct TupleConvertibleImpl<true, TupleTypes<>, TupleTypes<>>
        : public std::true_type
{
};

template<typename From_, typename To_,
         bool = TupleLike<typename std::remove_reference<From_>::type>::value,
         bool = TupleLike<typename std::remove_reference<To_>::type>::value>
struct TupleConvertible : public std::false_type
{
};

template<typename From_, typename To_>
struct TupleConvertible<From_, To_, true, true>
        : public TupleConvertibleImpl<
            TupleSize<typename std::remove_reference<From_>::type>::value
                == TupleSize<typename std::remove_reference<To_>::type>::value,
            typename MakeTupleTypes<From_>::type,
            typename MakeTupleTypes<To_>::type>
{
};

// TupleAssignable

template<bool IsSameSize_, typename To_, typename From_>
struct TupleAssignableImpl : public std::false_type
{
};

template<typename To0_, typename... ToRest_,
         typename From0_, typename... FromRest_>
struct TupleAssignableImpl<true, TupleTypes<To0_, ToRest_...>,
            TupleTypes<From0_, FromRest_...>>
        : public std::integral_constant<bool,
            std::is_assignable<To0_, From0_>::value
            && TupleAssignableImpl<true, TupleTypes<ToRest_...>,
                TupleTypes<FromRest_...>>::value>
{
};

template<>
struct TupleAssignableImpl<true, TupleTypes<>, TupleTypes<>>
        : public std::true_type
{
};

template<typename To_, typename From_,
         bool = TupleLike<typename std::remove_reference<To_>::type>::value,
         bool = TupleLike<typename std::remove_reference<From_>::type>::value>
struct TupleAssignable : public std::false_type
{
};

template<typename To_, typename From_>
struct TupleAssignable<To_, From_, true, true>
        : public TupleAssignableImpl<
            TupleSize<typename std::remove_reference<To_>::type>::value
                == TupleSize<typename std::remove_reference<From_>::type
                            >::value,
            typename MakeTupleTypes<To_>::type,
            typename MakeTupleTypes<From_>::type>
{
};

// TupleEqual

template<size_t I_>
struct TupleEqual
{
    template<typename Tuple0_, typename Tuple1_>
    bool operator()(const Tuple0_& t0, const Tuple1_& t1)
    {
        return TupleEqual<I_ - 1>()(t0, t1)
               && std::get<I_ - 1>(t0) == std::get<I_ - 1>(t1);
    }
};

template<>
struct TupleEqual<0>
{
    template<typename Tuple0_, typename Tuple1_>
    bool operator()(const Tuple0_&, const Tuple1_&)
    {
        return true;
    }
};

// TupleLess

template<size_t I_>
struct TupleLess
{
    template<typename Tuple0_, typename Tuple1_>
    bool operator()(const Tuple0_& t0, const Tuple1_& t1)
    {
        return TupleLess<I_ -1>()(t0, t1)
               || (!TupleLess<I_ - 1>()(t1, t0)
                   && std::get<I_ -1>(t0) < std::get<I_ - 1>(t1));
    }
};

template<>
struct TupleLess<0>
{
    template<typename Tuple0_, typename Tuple1_>
    bool operator()(const Tuple0_&, const Tuple1_&)
    {
        return false;
    }
};

}; // namespace detail

// tuple_size

template<typename Tuple_>
class tuple_size : public detail::TupleSize<Tuple_>
{
};

// tuple_element

template<size_t I_, typename Tuple_>
class tuple_element : public detail::TupleElement<I_, Tuple_>
{
};

// Tuple

template<typename... Ts_>
class Tuple
{
public:
    Tuple(const Ts_&... t)
            : impl_(typename detail::MakeTupleIndexes<sizeof...(Ts_)>::type(),
                    typename detail::MakeTupleTypes<Tuple, sizeof...(Ts_)
                        >::type(),
                    typename detail::MakeTupleIndexes<0>::type(),
                    typename detail::MakeTupleTypes<Tuple, 0>::type(),
                    t...)
    {
    }

    template<typename... Us, typename std::enable_if<
                sizeof...(Us) <= sizeof...(Ts_)
                && detail::TupleConvertible<
                    typename detail::MakeTupleTypes<Tuple<Us...>>::type,
                    typename detail::MakeTupleTypes<Tuple,
                        sizeof...(Us) < sizeof...(Ts_)
                        ? sizeof...(Us)
                        : sizeof...(Ts_)>::type>::value,
                bool>::type = false>
    explicit Tuple(Us&&... u)
            : impl_(typename detail::MakeTupleIndexes<sizeof...(Us)>::type(),
                    typename detail::MakeTupleTypes<Tuple, sizeof...(Us)>::type(),
                    typename detail::MakeTupleIndexes<sizeof...(Ts_),
                        sizeof...(Us)>::type(),
                    typename detail::MakeTupleTypes<Tuple, sizeof...(Ts_),
                        sizeof...(Us)>::type(),
                    std::forward<Us>(u)...)
    {
    };

    template<typename OtherTuple, typename std::enable_if<
                detail::TupleConvertible<OtherTuple, Tuple>::value,
                bool>::type = false>
    Tuple(OtherTuple&& t)
            : impl_(std::forward<OtherTuple>(t))
    {
    }

    template<typename OtherTuple, typename std::enable_if<
                detail::TupleAssignable<Tuple, OtherTuple>::value,
                bool>::type = false>
    Tuple& operator=(OtherTuple&& t)
    {
        impl_.operator=(std::forward<OtherTuple>(t));
        return *this;
    }

    void swap(Tuple& t)
    {
        impl_.swap(t.impl_);
    }

private:
    typedef detail::TupleImpl<
                typename detail::MakeTupleIndexes<sizeof...(Ts_)>::type, Ts_...>
            Impl;
    Impl impl_;

    template<size_t I, typename... Ts>
    friend typename tuple_element<I, Tuple<Ts...>>::type& get(Tuple<Ts...>& t);

    template<size_t I, typename... Ts>
    friend const typename tuple_element<I, Tuple<Ts...>>::type& get(
            const Tuple<Ts...>& t);

    template<size_t I, typename... Ts>
    friend typename tuple_element<I, Tuple<Ts...>>::type&& get(
            Tuple<Ts...>&& t);
};

// Empty
template<>
class Tuple<>
{
public:
    Tuple()
    {
    }

    template<typename OtherTuple, typename std::enable_if<
                detail::TupleConvertible<OtherTuple, Tuple>::value,
                bool>::type = false>
    Tuple(OtherTuple&&)
    {
    }

    template<typename OtherTuple, typename std::enable_if<
                detail::TupleAssignable<Tuple, OtherTuple>::value,
                bool>::type = false>
    Tuple& operator=(OtherTuple&&)
    {
        return *this;
    }

    void swap(Tuple&)
    {
    }
};

// get

template<size_t I, typename... Ts>
inline typename tuple_element<I, Tuple<Ts...>>::tuple& get(Tuple<Ts...>& t)
{
    typedef typename tuple_element<I, Tuple<Ts...>>::type Type;
    return static_cast<detail::TupleLeaf<I, Type>&>(t.impl_).get();
}

template<size_t I, typename... Ts>
inline const typename tuple_element<I, Tuple<Ts...>>::type& get(
        const Tuple<Ts...>& t)
{
    typedef typename tuple_element<I, Tuple<Ts...>>::type Type;
    return static_cast<const detail::TupleLeaf<I, Type>&>(t.impl_).get();
}
template<size_t I, typename... Ts>
inline typename tuple_element<I, Tuple<Ts...>>::type&& get(Tuple<Ts...>&& t)
{
    typedef typename tuple_element<I, Tuple<Ts...>>::type Type;
    return static_cast<Type&&>(
            static_cast<detail::TupleLeaf<I, Type>&&>(t.impl_).get());
}

// swap

template<typename... Ts>
inline void swap(Tuple<Ts...>& a, Tuple<Ts...>& b)
{
    a.swap(b);
}

// Relational operators

template<typename... T0s_, typename... T1s_>
inline bool operator==(const Tuple<T0s_...>& t0, const Tuple<T1s_...>& t1)
{
    return detail::TupleEqual<sizeof...(T0s_)>()(t0, t1);
}

template<typename... T0s_, typename... T1s_>
inline bool operator!=(const Tuple<T0s_...>& t0, const Tuple<T1s_...>& t1)
{
    return !(t0 == t1);
}

template<typename... T0s_, typename... T1s_>
inline bool operator<(const Tuple<T0s_...>& t0, const Tuple<T1s_...>& t1)
{
    return detail::TupleLess<sizeof...(T0s_)>()(t0, t1);
}

template<typename... T0s_, typename... T1s_>
inline bool operator>(const Tuple<T0s_...>& t0, const Tuple<T1s_...>& t1)
{
    return t1 < t0;
}

template<typename... T0s_, typename... T1s_>
inline bool operator<=(const Tuple<T0s_...>& t0, const Tuple<T1s_...>& t1)
{
    return !(t0 < t1);
}

}; // namespae demonia

namespace std
{

// tuple_size

template<typename... Ts_>
class tuple_size<demonia::Tuple<Ts_...>>
        : public demonia::tuple_size<demonia::Tuple<Ts_...>>
{
};

template<typename... Ts_>
class tuple_size<const demonia::Tuple<Ts_...>>
        : public demonia::tuple_size<const demonia::Tuple<Ts_...>>
{
};

template<typename... Ts_>
class tuple_size<volatile demonia::Tuple<Ts_...>>
        : public demonia::tuple_size<volatile demonia::Tuple<Ts_...>>
{
};

template<typename... Ts_>
class tuple_size<const volatile demonia::Tuple<Ts_...>>
        : public demonia::tuple_size<const volatile demonia::Tuple<Ts_...>>
{
};

// tuple_element

template<size_t I_, typename... Ts_>
class tuple_element<I_, demonia::Tuple<Ts_...>>
        : public demonia::tuple_element<I_, demonia::Tuple<Ts_...>>
{
};

template<size_t I_, typename... Ts_>
class tuple_element<I_, const demonia::Tuple<Ts_...>>
        : public demonia::tuple_element<I_, const demonia::Tuple<Ts_...>>
{
};

template<size_t I_, typename... Ts_>
class tuple_element<I_, volatile demonia::Tuple<Ts_...>>
        : public demonia::tuple_element<I_, volatile demonia::Tuple<Ts_...>>
{
};

template<size_t I_, typename... Ts_>
class tuple_element<I_, const volatile demonia::Tuple<Ts_...>>
        : public demonia::tuple_element<I_,
            const volatile demonia::Tuple<Ts_...>>
{
};

}; // namespace std

#endif // DEMONIA_SRC_TUPLE_HH
