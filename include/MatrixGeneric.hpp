#ifndef __MATRIX_GENERIC
#define __MATRIX_GENERIC

#include <cstdint>
#include <stdexcept>
#include <vector>

template <typename T> class MatrixGeneric
{
  public:
    explicit MatrixGeneric() = default;
    explicit MatrixGeneric(uint32_t height, uint32_t width)
        : _height(height), _width(width)
    {
        _data.resize(_height * _width);
    }

    MatrixGeneric(const MatrixGeneric &) = default;
    MatrixGeneric &operator=(const MatrixGeneric &) = default;

    MatrixGeneric(MatrixGeneric &&other) : MatrixGeneric()
    {
        *this = std::move(other);
    };

    MatrixGeneric &operator=(MatrixGeneric &&other) 
    {
        if (&other != this)
        {
            _height = other._height;
            _width = other._width;
            _data = std::move(other._data);
            other._height = 0;
            other._width = 0;
        }
        return *this;
    }

    virtual ~MatrixGeneric() = default;

    T& get(uint32_t x, uint32_t y)
    {
        return const_cast<T&>(
            const_cast<const MatrixGeneric<T> *>(this)->get(x, y)
                );
    }

    const T& get(uint32_t x, uint32_t y) const
    {
        auto idx = x * _width + y;
        if (idx >= _width * _height)
        {
            throw std::range_error("Indexing is out of range");
        }

        return _data[x*_width + y];
    }

    uint32_t height() const noexcept
    {
        return _height;
    }

    uint32_t width() const noexcept
    {
        return _width;
    }

  private:
    uint32_t _height, _width;
    std::vector<T> _data;
};

#endif
