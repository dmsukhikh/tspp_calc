#ifndef __MATRIX_OPS
#define __MATRIX_OPS

#include <stdexcept>
#include <cstdint>
#include <string>

// forward declaration
template <typename T> class MatrixGeneric;

template <typename A, typename B>
using AddType = decltype(std::declval<A>() + std::declval<B>());

template <typename A, typename B>
using SubsType = decltype(std::declval<A>() - std::declval<B>());

template <typename A, typename B>
using MulType = decltype(std::declval<A>() * std::declval<B>());

template <typename First, typename Second>
MatrixGeneric<AddType<First, Second>> operator+(const MatrixGeneric<First> &a,
                                                const MatrixGeneric<Second> &b)
{
    using RetType = MatrixGeneric<AddType<First, Second>>;

    if (a.height() != b.height() || a.width() != b.width())
    {
        std::string what = "Sizes of matrices aren't equal: ";
        what += std::to_string(a.height()) + "x" + std::to_string(a.width());
        what += " != ";
        what += std::to_string(b.height()) + "x" + std::to_string(b.width());
        throw std::range_error(what);
    }

    RetType out(a.height(), b.height());
    for (uint32_t i = 0; i < out.height() * out.width(); ++i)
    {
        out._data[i] = a._data[i] + b._data[i]; 
    }

    return out;
}

template <typename G, typename Scalar>
MatrixGeneric<MulType<Scalar, G>> operator*(Scalar s, const MatrixGeneric<G> &a)
{
    MatrixGeneric<MulType<Scalar, G>> out(a.height(), a.width());
    for (uint32_t i = 0; i < out.height() * out.width(); ++i)
    {
        out._data[i] = a._data[i] * s; 
    }
    return out;
}

template<typename A, typename B>
MatrixGeneric<SubsType<A, B>> operator-(const MatrixGeneric<A> &a, const MatrixGeneric<B> &b)
{
    // Код дублируется, потому что a + ((-1) * b) может привести к 
    // неправильному выводу типа: скажем a, b хранят тип short. Тогда в 
    // результате мы получим тип int, потому что -1 типа int. static_cast в
    // теории может не сработать.
    // Возможно, стоит добавить generic функцию, принимающую std::function?

    using RetType = MatrixGeneric<SubsType<A, A>>;

    if (a.height() != b.height() || a.width() != b.width())
    {
        std::string what = "Sizes of matrices aren't equal: ";
        what += std::to_string(a.height()) + "x" + std::to_string(a.width());
        what += " != ";
        what += std::to_string(b.height()) + "x" + std::to_string(b.width());
        throw std::range_error(what);
    }

    RetType out(a.height(), b.height());
    for (uint32_t i = 0; i < out.height() * out.width(); ++i)
    {
        out._data[i] = a._data[i] - b._data[i]; 
    }

    return out;
}

template <typename A, typename B>
MatrixGeneric<AddType<MulType<A, B>, MulType<A, B>>>
operator*(const MatrixGeneric<A> &a, const MatrixGeneric<B> &b)
{
    using RetType = AddType<MulType<A, B>, MulType<A, B>>;

    if (a.width() != b.height())
    {
        std::string what =
            "Sizes of matrices aren't compatible for multiplication: ";
        what += std::to_string(a.height()) + "x" + std::to_string(a.width());
        what += " and ";
        what += std::to_string(b.height()) + "x" + std::to_string(b.width());
        throw std::range_error(what);
    }

    MatrixGeneric<RetType> out(a.height(), b.width());
    for (uint32_t i = 0; i < a.height(); ++i)
    {
        for (uint32_t j = 0; j < b.width(); ++j)
        {
            RetType sm = 0;
            for (uint32_t k = 0; k < a.width(); ++k)
            {
                sm += a.get(i, k) * b.get(k, j); 
            }
            out.get(i, j) = sm;
        }
    }

    return out;
}

#endif
