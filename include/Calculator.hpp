#ifndef __CALCULATOR
#define __CALCULATOR
#include <MatrixGeneric.hpp>
#include <cstdint>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <stdexcept>

/*
 * Формат файла с матрицами:
 * [Операция] [Аргументы операции]
 * [Высота] [Ширина]
 * [Через пробел члены строк матриц]
 * ...
 * [Высота] [Ширина]
 * [Через пробел члены строк матриц]
 * ... сколько нужно матриц для операции ...
 */

template <typename T> T _conversionFromString(std::string &i) { return T(i); }
template <> int _conversionFromString<int>(std::string &i);
template <> float _conversionFromString<float>(std::string &i);
template <> double _conversionFromString<double>(std::string &i);
template <> long long _conversionFromString(std::string &i);

// Получить следующий элемент типа T из потока
template <typename T>
T getFromStream(std::basic_istream<char, std::char_traits<char>> &stream,
                std::string msg)
{
    std::string i;
    if (!(stream >> i))
    {
        throw std::runtime_error("not enough args or elements");
    }

    try
    {
        return _conversionFromString<T>(i);
    }
    catch (...)
    {
        throw std::runtime_error(msg);
    }
}

template <typename Type> class Calculator
{
    struct commandInfo
    {
        size_t operands;
        std::function<MatrixGeneric<Type>()> func;
    };

    /*
     * Хитрая python-way штука. С каждой коммандой ассоциирована структура.
     * Первое поле - количество операндов операции, а вот вторая -
     * непосредственно операция. Таким образом, мы можем вызвать требуемую
     * функцию, просто написав commands[opArgs[0]].func(). По-хорошему, function
     * должна возвращать или union или std::variant. Также, стоит отметить, что
     * функции не должны использоваться внутри конструктора Calculator,
     * поскольку commands использует поля, заполняемые там
     * */
    std::unordered_map<std::string, commandInfo> commands = {
        {"+", {2, [this]() { return matrices[0] + matrices[1]; }}},
        {"-", {2, [this]() { return matrices[0] - matrices[1]; }}},
        {"/", {2, [this]() { return matrices[0] / matrices[1]; }}},
        {"*", {2, [this]() { return matrices[0] * matrices[1]; }}},
        {"det",
         {1, [this]() { return MatrixGeneric<Type>{{matrices[0].det()}}; }}},
        {"inv", {1, [this]() { return matrices[0].inverse(); }}},
        {"rk",
         {1,
          [this]()
          {
              return MatrixGeneric<Type>{{static_cast<Type>(matrices[0].rk())}};
          }}},
        {"t", {1, [this]() { return matrices[0].transpose(); }}},
        {"pow",
         {1, [this]() { return matrices[0].pow(std::stoul(opArgs[1])); }}},
    };

    std::unordered_map<std::string, size_t> commandsWithArgs = {{"pow", 1}};
    std::vector<MatrixGeneric<Type>> matrices;
    std::vector<std::string> opArgs;

  public:
    Calculator(char **argv) : opArgs(1)
    {
        // Открытие файла
        std::ifstream matrixData(argv[1]);
        if (!matrixData.is_open())
        {
            std::cout << "[calc] error: invalid file. See README.md for info"
                      << std::endl;
            throw std::exception();
        }

        // Парсинг информации об операции + аргументы
        {
            std::string operationString;
            std::getline(matrixData,
                         operationString); // операция + аргументы
            std::stringstream itParsed(operationString);
            itParsed >> opArgs[0];

            if (commands.count(opArgs[0]) == 0)
            {
                std::cout << "[calc] error: invalid operation \"" << opArgs[0]
                          << "\".  See README.md for info" << std::endl;
                throw std::exception();
            }
            else
            {
                std::string temp;
                while (itParsed >> temp)
                {
                    opArgs.push_back(temp);
                }

                if (opArgs.size() - 1 < commandsWithArgs[opArgs[0]])
                {
                    std::cout
                        << "[calc] error: not enough arguments for operation "
                        << opArgs[0] << ". " << commandsWithArgs[opArgs[0]]
                        << " expected, but " << opArgs.size() - 1
                        << " was given." << std::endl;
                    throw std::exception();
                }
            }
        }

        try
        {
            // Парсинг матриц
            for (size_t idx = 0; idx < commands[opArgs[0]].operands; ++idx)
            {
                matrices.emplace_back(
                    static_cast<uint64_t>(getFromStream<int>(
                        matrixData, "error in getting width")),
                    static_cast<uint64_t>(getFromStream<int>(
                        matrixData, "error in getting height")));

                for (uint32_t i = 0; i < matrices[idx].height(); ++i)
                {
                    for (uint32_t j = 0; j < matrices[idx].width(); ++j)
                    {
                        matrices[idx].get(i, j) = getFromStream<Type>(
                            matrixData,
                            "error in getting elements of the matrix");
                    }
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << "[calc] error: " << e.what() << std::endl;
            throw e;
        }
    }

    MatrixGeneric<Type> getResult() { return commands[opArgs[0]].func(); }
};

template <typename T> void mainRoutine(char **argv)
{
    Calculator<T> a(argv);
    MatrixGeneric<T> res;

    try
    {
        res = a.getResult();
    }
    catch (basic_matrix_exception &e)
    {
        std::cout << "[calc] error while computing: " << e.what() << std::endl;
        throw e;
    }

    for (uint32_t i = 0; i < res.height(); ++i)
    {
        for (uint32_t j = 0; j < res.width(); ++j)
        {
            std::cout << res.get(i, j) << " ";
        }
        std::cout << std::endl;
    }
}
#endif
