#include <functional>
#include <gtest/gtest.h>
#include "Exceptions.hpp"
#include "MatrixGeneric.hpp"

// Test suite для тестирования методов MatrixGeneric

TEST(TestFuncs, TestDet)
{
    // Поиск определителя
    
    MatrixGeneric<int> a = {{1, 4, 0, 12},{-1, -2, 1, 1},{0, 1, 1, -1},{7, 0, 1, 2}};
    EXPECT_NEAR(a.det(), 325, 10e-6);
    EXPECT_EQ((3 * a).det(), 325 * 81);

    MatrixGeneric<int> b = {{1, 7, 0},{-1, -2, 0},{1, 3, 5}};
    EXPECT_NEAR(b.det(), 25, 10e-6);

    MatrixGeneric<int> rect = {{1, 3, 0, 2}, {1, 0, 1, 2}, {1, 2, 3, 1}};
    EXPECT_THROW(rect.det(), matrix_bad_det);

    MatrixGeneric<int> empty;
    EXPECT_NEAR(empty.det(), 1, 10e-6);

    MatrixGeneric<int> diag = {{2, 3, 2},{0, 1, 2},{0, 0, 4}};
    EXPECT_NEAR(diag.det(), 2*1*4, 10e-6);

    MatrixGeneric<int> zerodet = {{1, 1, 2, 2},{0, 1, 0, 0},{0, 0, 1, 0},{1, 3, 2, 2}};
    EXPECT_NEAR(zerodet.det(), 0, 10e-6);
    EXPECT_NEAR(MatrixGeneric<int>::eye(4).det(), 1, 10e-6);
}

TEST(TestFuncs, TestRk)
{
    EXPECT_EQ(MatrixGeneric<int>::eye(5).rk(), 5);

    MatrixGeneric<int> a = {{1, 2, 3}, {4, 5, 6}, {7, 8, 8}};
    EXPECT_EQ(a.rk(), 3);

    MatrixGeneric<int> b = {{1, 4, 0, 12},{-1, -2, 1, 1},{0, 1, 1, -1},{7, 0, 1, 2}};
    EXPECT_EQ(b.rk(), 4);

    MatrixGeneric<int> c = {{1, 0, 0, 0, 12}, {0, 1, 0, 11, -11}, {0, 0, 1, -2, -5}};
    EXPECT_EQ(c.rk(), 3);
    
    MatrixGeneric<int> d = {{1, 0},{0, 1},{1, 1},{0, 0},{1, 1}}; 
    EXPECT_EQ(d.rk(), 2);

    MatrixGeneric<int> e = {{1, 1, 1}, {0, 1, 0}, {2, 5, 2}};
    EXPECT_EQ(e.rk(), 2);
    EXPECT_EQ(MatrixGeneric<int>(3, 3).rk(), 0);
    EXPECT_EQ(MatrixGeneric<int>{}.rk(), 0);
}

TEST(TestFuncs, TestEye)
{
    // Тест, проверяющий корректность создания единичной матрицы
    auto E = MatrixGeneric<int>::eye(20);
    for (int i = 0; i < 20; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            if (i == j) EXPECT_EQ(E.get(i, j), 1);
            else        EXPECT_EQ(E.get(i, j), 0);
        }
    }
    auto smallE = MatrixGeneric<int>::eye(1);
    EXPECT_EQ(smallE, MatrixGeneric<int>{{1}});
}

TEST(TestFuncs, TestTranspose)
{
    MatrixGeneric<int> a = {{1, 4, 3, 2}, {1, 4, 5, 6}, {0, 1, 4, 0}};
    auto b = a.transpose();

    EXPECT_TRUE(a.width() == b.height() && a.height() == b.width());
    for (int i = 0; i < a.height(); ++i)
    {
        for (int j = 0; j < a.width(); ++j)
        {
            EXPECT_EQ(a.get(i, j), b.get(j, i));
        }
    }
}

TEST(TestFuncs, TestInverse)
{
    MatrixGeneric<int> bad_1 = {{1, 2, 3, 4, 5}},
                  bad_2 = {{1, 2},  {2, 4}};
    EXPECT_THROW(bad_1.inverse(), matrix_bad_inverse);
    EXPECT_THROW(bad_2.inverse(), matrix_bad_inverse);

    MatrixGeneric<int> good_1 = {{1, 2, 3}, {4, 5, 6}, {7, 8, 8}},
                  good_2 = {{1, 4, 0, 12},{-1, -2, 1, 1},{0, 1, 1, -1},{7, 0, 1, 2}};

    MatrixGeneric<float> out_1 = good_1 * good_1.inverse(),
                         out_2 = good_2 * good_2.inverse();

    // чудесный c++11 ww
    for (auto &testCase : {std::reference_wrapper<decltype(out_1)>(out_1),
                           std::reference_wrapper<decltype(out_2)>(out_2)})
    {
        for (int i = 0; i < testCase.get().height(); ++i)
        {
            for (int j = 0; j < testCase.get().width(); ++j)
            {
                if (i == j)
                    EXPECT_NEAR(testCase.get().get(i, j), 1, 10e-6);
                else
                    EXPECT_NEAR(testCase.get().get(i, j), 0, 10e-6);
            }
        }
    }
}
