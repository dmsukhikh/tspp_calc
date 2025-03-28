#ifndef __MATRIX_GENERIC
#define __MATRIX_GENERIC

#include <cstdint>
#include <vector>

template <typename T> class MatrixGeneric
{
  public:
    explicit MatrixGeneric() = default;
    explicit MatrixGeneric(uint32_t height, uint32_t width)
        : _height(height), _width(width)
    {
        _data.resize(_height);
        for (auto &raw: _data)
        {
            raw.resize(_width);
        }
    }

    MatrixGeneric(const MatrixGeneric &) = default;
    MatrixGeneric &operator=(const MatrixGeneric &) = default;

    MatrixGeneric(MatrixGeneric &&) = default;
    MatrixGeneric &operator=(MatrixGeneric &&) = default;

    virtual ~MatrixGeneric() = default;

    T& get(uint32_t x, uint32_t y)
    {
        return _data[x][y];
    }

    const T& get(uint32_t x, uint32_t y) const
    {
        return _data[x][y];
    }

  private:
    uint32_t _height, _width;
    std::vector<std::vector<T>> _data;
};

#endif
