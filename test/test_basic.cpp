#include <gtest/gtest.h>
#include <initializer_list>
#include "Exceptions.hpp"
#include "MatrixGeneric.hpp"
#include <string>
#include <sstream>

// Test suite для проверки корректности конструкторов/деструкторов, сравнения

TEST(TestBasic, DefaultConstructorTest)
{
    ASSERT_NO_THROW({
    MatrixGeneric<int> a; 
    MatrixGeneric<double> b;
    MatrixGeneric<short> c;
    });

    MatrixGeneric<int> a;
    EXPECT_EQ(a.height(), 0);
    EXPECT_EQ(a.width(), 0);
}

template <typename T>
using InitList = std::initializer_list<std::initializer_list<T>>;
TEST(TestBasic, InitializerListConstructorTest)
{
    // Проверка того, что матрицу можно создать из initializer_list. 
    // На вход матрице подается список списков. Если длины списков отличаются,
    // должно пробрасываться matrix_initialization_error.

    auto _defTest = []()
    {
        MatrixGeneric<int> a = {{1}};
        MatrixGeneric<short> b = {{1, 3, 4}, {1, 2, 7}};
        MatrixGeneric<float> c = {{1}, {2}, {3}};
    };
    ASSERT_NO_THROW(_defTest());

    InitList<int> init1 = {{1, 2, 3}, {4}},
                  init2 = {{3, 1, 0, 1, 0, 1}, {234, 234, 234}},
                  init3 = {{13, 4}, {5, 6}, {0 ,1}, {1}, {1, 4}};
    
    EXPECT_THROW(MatrixGeneric<int>{init1}, matrix_initialization_error)
        << "It must throw matrix_initialization_error";
    EXPECT_THROW(MatrixGeneric<int>{init2}, matrix_initialization_error)
        << "It must throw matrix_initialization_error";
    EXPECT_THROW(MatrixGeneric<int>{init3}, matrix_initialization_error)
        << "It must throw matrix_initialization_error";
}

TEST(TestBasic, WidthHeightConstructorTest)
{
    // Здесь мы тестируем конструктор, принимающий в качестве аргументов
    // ширину и высоту.
    // Ожидается, что при вызове MatrixGeneric(x, y) создастся нулевая
    // матрица x строк на y столбцов
    // В тесте также проверяются функции MatrixGeneric::width/height

    auto log = [](uint32_t x, uint32_t y, bool _isWidth = true)
    {
        std::stringstream format;
        format << "For matrix " << x << "x" << y << " ";
        if (_isWidth)
        {
            format << "width must be == " << y;
        }
        else
        {
            format << "height must be == " << x;
        }
        return std::string(std::istreambuf_iterator(format), {});
    };

    MatrixGeneric<int> a(3, 3);
    ASSERT_EQ(a.width(), 3) << log(3, 3);
    ASSERT_EQ(a.height(), 3) << log(3, 3, false);

    // Инициализация прямоугольной матрицы
    MatrixGeneric<int> b(3, 7);
    ASSERT_EQ(b.width(), 7) << log(3, 7);
    ASSERT_EQ(b.height(), 3) << log(3, 7, false);

    // Инициализация нулями
    EXPECT_THROW(MatrixGeneric<int>(0, 10), matrix_initialization_error)
        << "Matrix can't be initialized with only zero height";
    EXPECT_THROW(MatrixGeneric<int>(10, 0), matrix_initialization_error)
        << "Matrix can't be initialized with only zero width";
    EXPECT_NO_THROW(MatrixGeneric<int>(0, 0)) 
        << "It must be able to initialize trivial matrix 0x0";
}

TEST(TestBasic, EqualOperatorTest)
{
    // Тест проверяет оператор равенства/неравенства. Необходимо, чтобы для
    // матриц, имеющих одинаковое содержание + одинаковые размеры, но
    // инициализированых по-разному возвращалось true;

    MatrixGeneric<int> a(2, 3);
    MatrixGeneric b = {{0, 0, 0}, {0, 0, 0}};
    ASSERT_EQ(a, a) << "Matrix must be equal itself";
    ASSERT_EQ(b, b) << "Matrix must be equal itself";
    ASSERT_EQ(a, b) << "Matrices that was initialized with different "
                       "constructors but have the same content must be equal";
    ASSERT_EQ(b, a) << "Equal operator must be commutative";

    b = MatrixGeneric{{0, 1, 0}, {0, 0, 0}};
    EXPECT_NE(a, b);
    EXPECT_NE(b, a);

    MatrixGeneric<float> c = {{1, 2, 3, 4}, {5, 6, 7, 8}},
                         d = {{1, 2}, {3, 4}, {5, 6}, {7, 8}};
    EXPECT_NE(c, d) << "Content the same, but matrices have different sizes";

    MatrixGeneric<short> a_an(2, 3);
    EXPECT_EQ(a, a_an)
        << "Different types of matrices are comparable: e. g., int and short";
}

TEST(TestBasic, GetTest)
{
    // Проверка функции MatrixGeneric::get(x, y). При выходе за пределы матрицы,
    // пробрасывается matrix_bad_access

    MatrixGeneric test = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}},
                  cop  = {{1, 2, 0}, {4, 5, 6}, {-1, 8, 9}, {10, 11, 12}};
    // Проверка чтения
    for (int i = 0; i < 12; ++i)
    {
        EXPECT_EQ(test.get(i / test.width(), i % test.width()), i+1);
    }

    EXPECT_THROW(test.get(test.width(), test.height()), matrix_bad_access)
        << "Indexing must start from 0, as in arrays";
    EXPECT_THROW(test.get(test.width() + 1, test.height()), matrix_bad_access);
    EXPECT_THROW(test.get(test.width(), test.height() + 1), matrix_bad_access);
    EXPECT_THROW(test.get(test.width() + 1, test.height() + 1), matrix_bad_access);

    // Проверка записи
    test.get(0, 2) = 0;
    test.get(2, 0) = -1;
    EXPECT_EQ(test, cop);
}

TEST(TestBasic, GetTestConst)
{
    // Проверка функции const MatrixGeneric::get(x, y). При выходе за пределы
    // матрицы, пробрасывается matrix_bad_access

    const MatrixGeneric test = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}},
                  cop  = {{1, 2, 0}, {4, 5, 6}, {-1, 8, 9}, {10, 11, 12}};
    // Проверка чтения
    for (int i = 0; i < 12; ++i)
    {
        EXPECT_EQ(test.get(i / test.width(), i % test.width()), i+1);
    }

    EXPECT_THROW(test.get(test.width(), test.height()), matrix_bad_access)
        << "Indexing must start from 0, as in arrays";
    EXPECT_THROW(test.get(test.width() + 1, test.height()), matrix_bad_access);
    EXPECT_THROW(test.get(test.width(), test.height() + 1), matrix_bad_access);
    EXPECT_THROW(test.get(test.width() + 1, test.height() + 1), matrix_bad_access);
}

TEST(TestBasic, CopyTest)
{
    // Проверка копирования: конструктора и присваивания
    MatrixGeneric a = {{1, 2, 3}, {4, 5, 6}};
    MatrixGeneric copy_a = a;
    EXPECT_EQ(a, copy_a);
    EXPECT_EQ(copy_a, a);

    MatrixGeneric old_a = a;
    MatrixGeneric<int> assign_a;
    assign_a = a;
    a = a;
    EXPECT_EQ(a, assign_a);
    EXPECT_EQ(a, old_a) << "Assign to itself mustn't change the matrix";

    MatrixGeneric<int> b(10, 10);
    b = a;
    EXPECT_EQ(a, b);
}

TEST(TestBasic, MoveTest)
{
    // Проверка перемещения: конструктора и присваивания
    MatrixGeneric a = {{1, 2, 3}, {4, 5, 6}}, old_a = a;
    MatrixGeneric move_a = std::move(a);
    MatrixGeneric<int> b(10, 10);

    EXPECT_EQ(old_a, move_a);
    EXPECT_EQ(a, MatrixGeneric<int>{}) 
        << "Moving must erase source matrix";

    a = move_a;
    a = std::move(a);
    EXPECT_EQ(old_a, a)
        << "Moving to itself mustn't change the matrix";

    b = std::move(a);
    EXPECT_EQ(old_a, b);
}
