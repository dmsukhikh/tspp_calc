#ifndef __MATRIX_GENERIC
#define __MATRIX_GENERIC

#include <cstdint>
#include <initializer_list>
#include <algorithm>
#include <utility>
#include <vector>
#include "MatrixOperation.hpp"
#include "Exceptions.hpp"

/** \brief Класс, представляющий матрицу
 *  \details Класс, представляющий прямоугольную матрицу и реализующий основные
 * математические операции над ними.
 *  \tparam T Тип элементов матрицы. Тип должен быть:
 *   - Конструируемым
 *   - Иметь конструктор по умолчанию
 *   - Поддерживать основные арифметические операции: сложение, умножение,
 * деление и вычитание.
 */
template <typename T> class MatrixGeneric
{
  public:
    /**
     * \brief Конструктор по умолчанию
     * \details Создает пустую матрицу размером 0x0
     */
    explicit MatrixGeneric() = default;

    /**
     * \brief Конструктор, создающий матрицу с заданным размером
     * \details Создается матрица размером **height** x **width**
     *
     * \param height Высота матрицы
     * \param width Ширина матрицы
     *
     * \exception matrix_initialization_error В случае, если ширина нулевая, а
     * высота - нет, и наоборот. Так, нельзя создать матрицу 0x10 или 2x0.
     */
    explicit MatrixGeneric(uint32_t height, uint32_t width)
        : _height(height), _width(width)
    {
        if ((_height == 0 && _width != 0) || (_width == 0 && _height != 0))
        {
            throw matrix_initialization_error(
                "Only both height and width can be zero");
        }
        _data.resize(_height * _width);
    }

    MatrixGeneric(const MatrixGeneric &) = default;
    MatrixGeneric &operator=(const MatrixGeneric &) = default;

    /**
     * \brief Конструктор матрицы из initializer_list
     * \details Конструктор создает матрицу из списка списков инициализации.
     * Пример использования:
     * \code
     * MatrixGeneric<int> m = {{1, 2, 3},
     *                         {4, 5, 6},
     *                         {7, 8, 9}};
     * \endcode
     *
     * \param nums Список списков инициализации
     * \exception matrix_initialization_error Выбрасывается в двух случаях:
     *  - Список некорректный: все списки должны быть одинаковой длины
     *  - Ширина нулевая, а высота - нет, и наоборот. Так, нельзя создать
     * матрицу 0x10 или 2x0.
     */
    MatrixGeneric(const std::initializer_list<std::initializer_list<T>> &nums)
    {
        if (nums.size() == 0)
            return ;

        auto width = nums.begin()->size();
        auto height = nums.size();
        auto isValidList = std::all_of(nums.begin(), nums.end(),
                                       [width](const std::initializer_list<T> &raw)
                                       { return raw.size() == width; });
        if (!isValidList)
            throw matrix_initialization_error("Invalid initializer list");
        
        _width = width;
        _height = height;

        if ((_height == 0 && _width != 0) || (_width == 0 && _height != 0))
        {
            throw matrix_initialization_error(
                "Only both height and width can be zero");
        }
        
        for (auto &raw: nums)
        {
            for (auto &i: raw)
            {
                _data.push_back(i);
            }
        }
    }

    /**
     * \brief Создание единичной матрицы
     * \details Функция создает квадратную матрицу со стороной **size**, где на
     * главной диагонали находятся единицы.
     * \note Для использования этой функции тип **T** должен иметь конструктор
     * `T::T(int)`.
     *
     * \param size Длина стороны матрицы
     * \return Единичная матрицы
     */
    static MatrixGeneric eye(uint32_t size)
    {
        MatrixGeneric<T> out(size, size);
        for (uint32_t i = 0; i < size; ++i)
        {
            out.get(i, i) = T(1);
        }
        return out;
    }

    MatrixGeneric(MatrixGeneric &&other)
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

    ~MatrixGeneric() = default;

    /**
     * \copydoc get()
     */
    T& get(uint32_t x, uint32_t y)
    {
        return const_cast<T&>(
            const_cast<const MatrixGeneric<T> *>(this)->get(x, y)
                );
    }

    /**
     * \brief Получение элемента
     * \details Получение элемента матрицы, находящийся в строке **x** и столбце **y**
     * \note Нумерация строк и столбцов начинается с нуля
     *
     * \param x Строка элемента
     * \param y Столбец элемента
     * \return lvalue-ссылка на элемент матрицы
     * \throw matrix_bad_access Если взятие элемента выходит за пределы матрицы
     */
    const T& get(uint32_t x, uint32_t y) const
    {
        auto idx = x * _width + y;
        if (idx >= _width * _height)
        {
            throw matrix_bad_access("Indexing is out of range");
        }

        return _data[x*_width + y];
    }

    /**
     * \brief Получение высоты матрицы
     * \return Высота матрицы
     */
    uint32_t height() const noexcept
    {
        return _height;
    }

    /**
     * \brief Получение ширины матрицы
     * \return Ширина матрицы
     */
    uint32_t width() const noexcept
    {
        return _width;
    }

    /**
     * \brief Сложение двух матриц
     * \details Складывает две матрицы.
     *
     * Временная сложность: O(n), где n - количество элементов в матрице.
     *
     * \note Тип элементов возвращаемой матрицы вычисляется как тип,
     * получающийся в результате сложения двух элементов из разных матриц. Так,
     * если мы складываем MatrixGeneric<int> и MatrixGeneric<float>, то
     * получится MatrixGeneric<float>. Таким образом, можно складывать матрицы,
     * состоящие из элементов разных типов, но которые приводятся один к другому
     *
     * \tparam A Тип элеметов первой матрицы
     * \tparam B Тип элементов второй матрицы
     * \param a Первая матрицa
     * \param b Вторая матрица
     *
     * \exception matrix_bad_operation Если размеры матриц не совпадают
     * \return Сумма матриц
     */
    template <typename A, typename B>
    friend MatrixGeneric<AddType<A, B>> operator+(const MatrixGeneric<A> &a,
                                                  const MatrixGeneric<B> &b);

    /**
     * \brief Разность двух матриц
     * \details Вычитает одну матрицу из другой
     *
     * Временная сложность: O(n), где n - количество элементов в матрице.
     *
     * \note Тип вычисляется подобно тому, как и при \ref operator+() "сложении"
     * матриц. То есть, есть возможность вычитать матрицы разных типов.
     *
     * \tparam A Тип элеметов первой матрицы
     * \tparam B Тип элементов второй матрицы
     * \param a Первая матрицa
     * \param b Вторая матрица
     *
     * \return Разность матриц
     * \exception matrix_bad_operation Если размеры матриц не совпадают
     * \sa operator+()
     */
    template <typename A, typename B>
    friend MatrixGeneric<SubsType<A, B>> operator-(const MatrixGeneric<A> &a,
                                                   const MatrixGeneric<B> &b);

    /**
     * \brief Произведение двух матриц
     * \details Умножает одну матрицу на другую, используя определение умножения
     * матриц.
     *
     * Временная сложность: O(n^3), где n - длина стороны матрицы.
     *
     * \note Тип вычисляется подобно тому, как и при \ref operator+() "сложении"
     * матриц. То есть, есть возможность вычитать матрицы разных типов.
     * Конкретнее, тип элементов возвращаемой матрицы вычисляется как тип
     * выражения 'a.get(0,0) * b.get(0,0) + a.get(0,0) * b.get(0,0)`
     *
     * \tparam A Тип элеметов первой матрицы
     * \tparam B Тип элементов второй матрицы
     * \param a Первая матрицa
     * \param b Вторая матрица
     *
     * \return Сумма матриц
     * \exception matrix_bad_operation Если размеры матриц не позволяют произвести
     * умножение
     * \sa operator+()
     */
    template <typename A, typename B>
    friend MatrixGeneric<AddType<MulType<A, B>, MulType<A, B>>>
    operator*(const MatrixGeneric<A> &a, const MatrixGeneric<B> &b);

    /**
     * \brief Произведение матрицы на скаляр 
     * \details Умножает матрицу на скаляр 
     *
     * Временная сложность: O(n), где n - длина стороны матрицы.
     *
     * \note
     * - Тип вычисляется подобно тому, как и при \ref operator+() "сложении"
     * матриц
     * - Число должно идти первым. Выражение вида `MatrixGeneric<int>::eye(3) *
     * 2` не скомпилируется
     *
     * \tparam G Тип элеметов матрицы
     * \tparam Scalar Тип скаляра 
     * \param scalar Скаляр, на который умножается матрица 
     * \param a Mатрица
     *
     * \return Матрица, умноженная на скаляр
     * \sa operator+()
     */
    template <typename G, typename Scalar>
    friend MatrixGeneric<MulType<Scalar, G>>
    operator*(Scalar scalar, const MatrixGeneric<G> &a);

    /**
     * \brief Делит матрицу на другую
     * \details Делит первую матрицу на вторую. По определению, деление на
     * матрицу означает умножение на обратную, так что матрицы должны быть
     * квадратными, а делитель, к тому же, иметь ненулевой определитель
     *
     * Временная сложность: O(n^4), где n - длина стороны матрицы
     *
     * \note Тип элементов возвращаемой матрицы вычисляется подобно тому, как он
     * вычисляется при \ref operator+() "сложении" матриц. Однако, данный тип
     * вычисляется из выражения вида `a.get(0, 0) * (b.get(0, 0) / 1.0f)`.
     * Причина этому кроется в том, что результат деления двух целочисленных
     * матриц, например, будет матрица типа float - деление не замкнуто в целых
     * числах, поэтому мы сначала пытаемся поделить на float. С другой стороны,
     * это не будет работать, если элементы матрицы из какого-нибудь конечного
     * поля. Поэтому в будущем будет добавлена generic-версия деления (см. todo)
     *
     * \todo Добавить generic-версию деления, где пользователь может сам
     * выбрать, к какому типу будет приводиться результат деления
     *
     * \tparam A Тип элементов первой матрицы
     * \tparam B Тип элементов второй матрицы
     * \param a Матрица-делимое
     * \param b Матрица-делитель
     * \return Частное деления матрицы **a** на матрицу **b**
     *
     * \exception matrix_bad_operation Если делитель не квадратный
     * \exception <any> Выбрасывает то же самое, что и operator*(),
     * MatrixGeneric<T>::inverse()
     *
     * \sa operator+()
     * \sa operator*()
     * \sa MatrixGeneric<T>::inverse()
     */
    template <typename A, typename B>
    friend MatrixGeneric<MulType<A, DivType<B, float>>>
    operator/(const MatrixGeneric<A> &a, const MatrixGeneric<B> &b);

    /**
     * \brief Сравнение матриц
     * \details Определяет, равны ли две матрицы согласно математическому
     * определению
     *
     * \tparam A Тип элементов первой матрицы
     * \tparam B Тип элементов второй матрицы
     * \param a Первая матрица 
     * \param b Вторая матрица 
     * \return Равны ли две матрицы 
     */
    template <typename A, typename B>
    friend bool operator==(const MatrixGeneric<A> &a,
                           const MatrixGeneric<B> &b);

    /**
     * \brief Сравнение матриц
     * \details Определяет, не равны ли две матрицы согласно математическому
     * определению. Фактически, функция - отрицание \ref operator==()
     * "равенства" двух матриц
     *
     * \tparam A Тип элементов первой матрицы
     * \tparam B Тип элементов второй матрицы
     * \param a Первая матрица
     * \param b Вторая матрица
     * \return Равны ли две матрицы
     * \sa operator==()
     */
    template <typename A, typename B>
    friend bool operator!=(const MatrixGeneric<A> &a,
                           const MatrixGeneric<B> &b);

    /**
     * \brief Вычисление определителя матрицы
     * \details Метод вычисляет определитель матрицы, используя метод Гаусса
     * сведения матрицы к вернетреугольному виду.
     *
     * Временная сложность алгоритма: O(n^3), где n - длина стороны матрицы
     * \note
     * - Для использования данной функции тип T должен быть арифметическим, то
     * есть поддерживать арифметические операции. Также, тип T должен
     * приводиться к типу float, даже несмотря на то, что возвращаемый тип - T.
     * - Для пустой матрицы определитель принимается равным единице
     *
     * \todo Планируется сделать дополнительную перегрузку det, которая не
     * зависит от приведения к float
     *
     * \return Определитель матрицы
     * \throw matrix_bad_det Если матрица не квадратная
     */
    T det() const
    {
        if (_height != _width)
            throw matrix_bad_det("Matrix isn't square"); 

        if (_height == 0 && _width == 0)
            return 1; 

        return _gauss().second;    
    }

    /**
     * \brief Транспонирование матрицы
     * \details Метод вычисляет новую матрицу, транспонируя исходную, и
     * возвращает транспонированную копию. Исходная матрица **не меняется**.
     *
     * Временная сложность алгоритма: O(n), где n - количество элементов
     *
     * \return Транспонированная матрица
     */
    MatrixGeneric transpose() const
    {
        std::vector<T> _new_data(_width * _height);
        for (uint32_t i = 0; i < _height; ++i)
        {
            for (uint32_t j = 0; j < _width; ++j)
            {
                _new_data[j*_height + i] = _data[i*_width + j];
            }
        }

        MatrixGeneric<T> out(_width, _height);
        out._data = std::move(_new_data);
        return out;
    }

    /**
     * \brief Обращение матрицы
     * \details Метод вычисляет матрицу, обратную исходной, используя метод
     * алгебраических дополнений.
     *
     * Временная сложность алгоритма: O(n^4), где n - длина стороны матрицы
     *
     * \note Для использования данной функции тип T должен быть арифметическим,
     * то есть поддерживать арифметические операции. Также, тип T должен
     * приводиться к типу float
     *
     * \todo Планируется сделать дополнительную перегрузку inverse, которая не
     * зависит от приведения к float
     *
     * \return Матрица, обратная данной
     * \throw matrix_bad_inverse Если матрица не квадратная
     * \throw matrix_bad_inverse Если определитель матрицы равен нулю. В таком
     * случае, обратной матрицы к данной не существует
     * */
    MatrixGeneric<DivType<T, float>> inverse() const
    {
        if (_height != _width)
            throw matrix_bad_inverse("Matrix isn't square");

        MatrixGeneric<DivType<T, float>> out(_height, _width);

        auto delta = det();
        if (delta == 0)
            throw matrix_bad_inverse("Matrix determinant equals zero"); 

        for (uint32_t i = 0; i < _height; ++i)
        {
            for (uint32_t j = 0; j < _width; ++j)
            {
                out.get(i, j) = ((i + j) % 2 ? -1 : 1) * _cofactor(i, j).det();
            }
        }

        out = (1.f / delta) * out.transpose();
        return out;
    }

    /**
     * \brief Возведение матрицы в степень
     * \details Возвращает исходную матрицу, возведенную в степень **pow**. При
     * вычислении степени используется линейное перемножение матриц.
     *
     * Временная сложность: O(n^3 * pow), где n - длина стороны матрицы
     *
     * \param power Степень, в которую необходимо возвести матрицу
     * \return Исходная матрица в степени **power**
     * \exception matrix_bad_pow Если матрица не квадратная
     */
    MatrixGeneric pow(uint32_t power)
    {
        if (_height != _width)
            throw matrix_bad_pow("Matrix isn't square");

        if (power == 0)
            return MatrixGeneric<T>::eye(_height);

        MatrixGeneric<T> out(*this);

        while (power > 1)
        {
            out = out * *this;
            --power;
        }
        return out;
    }

    /**
     * \brief Вычисление ранга матрицы
     * \details Вычисление ранга матрицы методом Гаусса.
     *
     * Временная сложность: О(n^3), где n - длина стороны матрицы.
     *
     * \note Поскольку при вычислении ранга используется метод Гаусса, в силе
     * все ограничения, накладываемые на метод \ref det()
     *
     * \return Ранг матрицы
     * \sa det()
     */
    uint32_t rk() const
    {
        return _gauss().first;
    }

  private:
    uint32_t _height{0}, ///< Высота матрицы
        _width{0};       ///< Ширина матрицы

    std::vector<T> _data; ///< Элементы матрицы, хранятся в одномерном массиве

    /**
     * \brief Вычисление алгебраического дополнения матрицы
     * \details Вычисляет алгебраическое дополнение, удаляя i-тую строку и j-тый
     * столбец
     *
     * Временная сложность - O(n), где n - количество элементов
     *
     * \param i строка, которую нужно удалить
     * \param j столбец, который нужно удалить
     * \return Алгебраическое дополнение
     * \throw matrix_bad_access Если удаляемая строка или столбец выходит за
     * пределы матрицы
     */
    MatrixGeneric _cofactor(uint32_t i, uint32_t j) const
    {
        if (i >= _height)
            throw matrix_bad_access("i is out of range");
        if (j >= _width)
            throw matrix_bad_access("j is out of range");

        MatrixGeneric out = *this;

        auto cnt = 0;
        auto deleter = [&cnt, i, j, this](const T&)
        {
            auto pred = cnt / _width == i || cnt % _width == j;
            ++cnt;
            return pred;
        };

        auto to_del =
            std::remove_if(out._data.begin(), out._data.end(), deleter);
        out._data.erase(to_del, out._data.end());
        out._height--;
        out._width--;

        return out;
    }

    /**
     * \brief Метод Гаусса
     * \details Метод Гаусса =). Вычисляет одновременно и определитель, и ранг.
     * \todo Сделать кэширование для метода Гаусса
     * \return `std::pair`. В first лежит ранг, в second - определитель
     */
    std::pair<uint32_t, T> _gauss() const
    {
        // Метод Гаусса приведения матрицы к верхнетреугольному виду. 1 - ранг,
        // 2 - определитель

        char sign = 1;

        MatrixGeneric<DivType<float, T>> _copy(_height, _width);
        for (uint32_t i = 0; i < _width * _height; ++i)
        {
            _copy.get(i/_width, i%_width) = _data[i];
        }
        uint32_t rank = std::min(_height, _width);

        for (uint32_t row = 0; row < rank; row++)
        {
            if (_copy.get(row, row) != 0)
            {
                for (uint32_t targetRow = 0; targetRow < _height; targetRow++)
                {
                    if (targetRow != row)
                    {
                        auto mult = (float)_copy.get(targetRow, row) /
                                    _copy.get(row, row);
                        for (uint32_t i = 0; i < rank; i++)
                            _copy.get(targetRow, i) -= mult * _copy.get(row, i);
                    }
                }
            }
            else
            {
                bool reduce = true;
                for (uint32_t i = row + 1; i < _height; i++)
                {
                    if (_copy.get(i, row) != 0)
                    {
                        for (uint32_t targetRow = 0; targetRow < rank;
                             targetRow++)
                        {
                            std::swap(_copy.get(row, targetRow),
                                      _copy.get(i, targetRow));
                        }
                        reduce = false;
                        sign *= -1;
                        break;
                    }
                }

                if (reduce)
                {
                    rank--;
                    sign = 0;
                    for (uint32_t i = 0; i < _height; i ++)
                        _copy.get(i, row) = _copy.get(i, rank);
                }
                row--;
            }
        }
        
        T absdet = (_width == 0 && _height == 0) ? 0 : 1;
        for (uint32_t i = 0; i < std::min(_width, _height); ++i)
        {
            absdet *= _copy.get(i, i);
        }

        return {rank, absdet*sign};
    }
};

#endif
