#ifndef __CALCULATOR
#define __CALCULATOR
#include <MatrixGeneric.hpp>
#include <cstdint>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

/**
 * \file Calculator.hpp
 * Файл, содержащий класс Calculator и некоторые вспомогательные функции
 * \author dmsukhikh
 */

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

/**
 * \brief Преобразование строки в объект типа **T**
 * \details Вспомогательная функция, которая используется в getFromStream
 * \param i Строка, из которой необходимо получить объект типа **T**
 * \return Выражение вида `T(i)`
 * \sa getFromStream()
 */
template <typename T> T _conversionFromString(std::string &i) { return T(i); }

template <> int _conversionFromString(std::string &i);

template <> float _conversionFromString(std::string &i);

template <> double _conversionFromString(std::string &i);

template <> long long _conversionFromString(std::string &i);

/**
 * \brief Безопасное получение следующего объекта из потока
 * \details Извлекает из потока **stream** следующий объект типа **T**. В случае
 * ошибки преобразования типа выбрасывает исключение с сообщением **msg**.
 *
 * \note Для произвольного типа данных **T** используется конструктор
 * `T::T(std::string)`. Для некоторых встроенных типов данных есть спецификации,
 * использующие функции типа `std::stoi()`.
 *
 * \param stream Поток вывода, из которого берется информация
 * \param msg Сообщение при ошибке
 * \return Полученный из потока объект
 * \throw std::runtime_error В случае, если происходит попытка получить объект
 * из пустого потока или если происходит ошибка при преобразовании строки в
 * объект типа T
 * */
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

/**
 * \brief Класс для работы с выражениями из файла
 * \details Класс Calculator парсит задаваемый пользователем файл и вычисляет
 * результат
 * \tparam Type Тип данных, необходимый пользователю
 */
template <typename Type> class Calculator
{
    struct commandInfo
    {
        size_t operands;
        std::function<MatrixGeneric<Type>()> func;
    };
    ///< Структура, хранящая количество операндов операции и функцию, которую
    ///< необходимо вызывать

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
    ///< Сколько аргументов требует каждая операция
    std::vector<MatrixGeneric<Type>>
        matrices;                    ///< Здесь хранятся распарсенные матрицы
    std::vector<std::string> opArgs; ///< Распарсенные операция и ее аргументы

  public:
    /**
     * \brief Парсит вводимый файл
     * \details Получает данные о матрицах и операции над ними из файла
     * \param argv аргументы командной строки
     * \throw std::exception Если файл не существует или информация в нем
     * некорректная
     */
    Calculator(char **argv) : opArgs(1)
    {
        // Открытие файла
        std::ifstream matrixData(argv[1]);
        if (!matrixData.is_open())
        {
            std::cout
                << "[tspp_calc] error: invalid file. See README.md for info"
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
                std::cout << "[tspp_calc] error: invalid operation \""
                          << opArgs[0] << "\".  See README.md for info"
                          << std::endl;
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
                    std::cout << "[tspp_calc] error: not enough arguments for "
                                 "operation "
                              << opArgs[0] << ". "
                              << commandsWithArgs[opArgs[0]]
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
            std::cout << "[tspp_calc] error: " << e.what() << std::endl;
            throw e;
        }
    }

    /**
     * \brief Вычисляет результат операции
     * \return Результат операции
     * \throw <Any> Если происходит ошибка в операции
     */
    MatrixGeneric<Type> getResult() { return commands[opArgs[0]].func(); }
};

/**
 * \brief Основная процедура калькулятора
 * \details Калькулятор принимает аргументы, открывает файл, парсит его и
 * печатает результат
 * \param argv аргументы командной строки
 * \return None
 * \throw basic_matrix_exception В случае неудачного парсинга файла
 */
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
        std::cout << "[tspp_calc] error while computing: " << e.what()
                  << std::endl;
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
