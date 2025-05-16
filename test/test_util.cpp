#include "MatrixGeneric.hpp"
#include <functional>
#include <gtest/gtest.h>
#include <Calculator.hpp>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

struct Args
{
    char **args;
    std::string _file;
    Args(std::string file) : _file(file)
    {
        args = new char *[2];
        args[0] = new char[1]{'a'};
        args[1] = const_cast<char *>(_file.c_str());
    }

    ~Args()
    {
        delete [] args[0];
        delete [] args;
    }
};

void generateFileBinaryOp(MatrixGeneric<float>& a, MatrixGeneric<float>& b,
                          std::string op, std::vector<std::string> &&args)
{
    std::ofstream file("test_suite/valid.txt");
    file << op; 
    for (auto &i: args)
    {
        file << " " << i;
    }
    file << std::endl;

    for (auto &m : {std::ref(a), std::ref(b)})
    {
        file << m.get().height() << " " << m.get().width() << std::endl;
        for (uint32_t i = 0; i < m.get().height(); ++i)
        {
            for (uint32_t j = 0; j < m.get().width(); ++j)
            {
                file << m.get().get(i, j) << " ";
            }
            file << std::endl;
        }
    }
}

TEST(TestUtil, TestGetFromStreamString)
{
    std::stringstream test("a b c d");
    std::string acc;
    acc += getFromStream<std::string>(test, "");
    acc += getFromStream<std::string>(test, "");
    acc += getFromStream<std::string>(test, "");
    acc += getFromStream<std::string>(test, "");
    
    EXPECT_EQ(acc, "abcd");
    EXPECT_ANY_THROW(getFromStream<std::string>(test, ""));
}

TEST(TestUtil, TestGetFromStreamInt)
{
    std::vector<int> data = {1, 1, 1, 1, -1, 5, 3};
    std::string dataString;
    for (int i: data) dataString += std::to_string(i) + " ";

    std::stringstream test(dataString);
    int sm = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        sm += getFromStream<int>(test, "");
    }

    EXPECT_EQ(sm, std::accumulate(data.begin(), data.end(), 0));
}

TEST(TestUtil, TestGetFromStreamFloat)
{
    std::vector<float> data = {1.2, 1.5, -10.1, 1.123, -1.2, 5.1, 3.2, 52};
    std::string dataString;
    for (float i: data) dataString += std::to_string(i) + " ";

    std::stringstream test(dataString);
    int sm = 0;
    for (size_t i = 0; i < data.size(); ++i)
    {
        sm += getFromStream<float>(test, "");
    }

    EXPECT_NEAR(sm, std::accumulate(data.begin(), data.end(), 0), 10e-6);
}

TEST(TestUtil, TestCalcWrongFile)
{
    Args args("bobic.txt");
    EXPECT_ANY_THROW(Calculator<float>(args.args)); 
}

TEST(TestUtil, TestCalcNotEnoughArgsForOperation)
{
    Args args("test_suite/badPow.txt");
    EXPECT_ANY_THROW(Calculator<float>(args.args)); 
}

TEST(TestUtil, TestCalcNotEnoughMatricesForOperation)
{
    Args args("test_suite/badAdd.txt");
    EXPECT_ANY_THROW(Calculator<float>(args.args)); 
}

TEST(TestUtil, TestCalcNotEnoughElementsForMatrix)
{
    Args args("test_suite/notCompleteMatrix.txt");
    EXPECT_ANY_THROW(Calculator<float>(args.args)); 
}

TEST(TestUtil, TestCalcValidBinaryOps)
{
    MatrixGeneric<float> a = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}},
                         b = {{1, 0, 1}, {2, 3, 4}, {5, 6, 7}};

    std::unordered_map<std::string, std::function<MatrixGeneric<float>()>> ops =
        {
            {"+", [a, b]() { return a + b; }},
            {"-", [a, b]() { return a - b; }},
            {"/", [a, b]() { return a / b; }},
            {"*", [a, b]() { return a * b; }},
        };

    for (auto op: ops)
    {
        generateFileBinaryOp(a, b, op.first, {});
        Args args("test_suite/valid.txt");
        EXPECT_EQ(Calculator<float>(args.args).getResult(), op.second()); 
    }
}

TEST(TestUtil, TestCalcValidUnaryOps)
{
    MatrixGeneric<float> a = {
        {1, 2, 3, 4}, {4, 5, -6, 7}, {7, 8, 9, -10}, {4, 0, 4, 0}};

    std::unordered_map<std::string, std::function<MatrixGeneric<float>()>> ops =
        {
            {"det", [a]() -> MatrixGeneric<float> { return {{a.det()}}; }},
            {"inv", [a]() -> MatrixGeneric<float> { return {{a.inverse()}}; }},
            {"rk", [a]() -> MatrixGeneric<float>
             { return {{ static_cast<float>(a.rk()) }}; }},
            {"t", [a]() -> MatrixGeneric<float> { return a.transpose(); }}
        };

    for (auto op: ops)
    {
        MatrixGeneric<float> empty;
        generateFileBinaryOp(a, empty, op.first, {});
        Args args("test_suite/valid.txt");
        EXPECT_EQ(Calculator<float>(args.args).getResult(), op.second()); 
    }
}

TEST(TestUtil, TestCalcValidPow)
{
    MatrixGeneric<float> a = {
        {1, 2, 3, 4}, {4, 5, -6, 7}, {7, 8, 9, -10}, {4, 0, 4, 0}};

    for (int i = 1; i < 10; ++i)
    {
        MatrixGeneric<float> empty;
        generateFileBinaryOp(a, empty, "pow", {std::to_string(i)});
        Args args("test_suite/valid.txt");
        EXPECT_EQ(Calculator<float>(args.args).getResult(), a.pow(i));
    }
}
