#include <gtest/gtest.h>

#include <MatrixGeneric.hpp>

TEST(TestOps, TestAddition)
{
    MatrixGeneric a = {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, {5, 6, 1, 2, 3}};
    MatrixGeneric b = {{0, 2, 1, 4, 5}, {-10, 2, 30, 4, 25}, {5, -6, 1, 2, 3}};

    for (uint32_t i = 0; i < a.height(); ++i)
    {
        for (uint32_t j = 0; j < a.width(); ++j)
        {
            EXPECT_EQ((a + b).get(i, j), a.get(i, j) + b.get(i, j))
                << i << " " << j;
        }
    }

    EXPECT_THROW(a + MatrixGeneric<int>::eye(3), matrix_bad_operation);
}

TEST(TestOps, TestSubstitution)
{
    MatrixGeneric a = {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, {5, 6, 1, 2, 3}};
    MatrixGeneric b = {{0, 2, 1, 4, 5}, {-10, 2, 30, 4, 25}, {5, -6, 1, 2, 3}};

    for (int i = 0; i < a.height(); ++i)
    {
        for (int j = 0; j < a.width(); ++j)
        {
            EXPECT_EQ((a - b).get(i, j), a.get(i, j) - b.get(i, j));
        }
    }

    EXPECT_THROW(a - MatrixGeneric<int>::eye(3), matrix_bad_operation);
}

TEST(TestOps, TestMultiplication)
{
    EXPECT_THROW(MatrixGeneric<int>::eye(3) * MatrixGeneric<int>::eye(4),
                 matrix_bad_operation);
    MatrixGeneric a = {{1, 3, 5, 1}, {-1, 3, -2, 10}},
                  b = {{1, 3}, {-1, 1}, {1, 0}, {10, 1}},
                  axb = {{13, 7}, {94, 10}},
                  bxa = {{-2, 12, -1, 31},
                         {-2, 0, -7, 9},
                         {1, 3, 5, 1},
                         {9, 33, 48, 20}};
    EXPECT_EQ(axb, a * b);
    EXPECT_EQ(bxa, b * a);
}

TEST(TestOps, TestDivision)
{
    MatrixGeneric a = {{1, 2, 3}, {4, 5, 6}, {7, 8, 8}};
    auto testCase = a / a;

    for (int i = 0; i < testCase.height(); ++i)
    {
        for (int j = 0; j < testCase.width(); ++j)
        {
            if (i == j)
                EXPECT_NEAR(testCase.get(i, j), 1, 10e-6);
            else
                EXPECT_NEAR(testCase.get(i, j), 0, 10e-6);
        }
    }
}
