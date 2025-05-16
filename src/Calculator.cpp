#include <Calculator.hpp>

/**
 * \file Calculator.cpp
 * Файл содержит специализации _conversionFromString для некоторых встроенных
 * типов. Конструктор типа Т заменен в них на функции вида std::stoi()
 * \author dmsukhikh
 */

template <> int _conversionFromString<int>(std::string &i)
{
    return std::stoi(i);
}
template <> float _conversionFromString<float>(std::string &i)
{
    return std::stof(i);
}
template <> double _conversionFromString<double>(std::string &i)
{
    return std::stod(i);
}
template <> long long _conversionFromString(std::string &i)
{
    return std::stoll(i);
}
