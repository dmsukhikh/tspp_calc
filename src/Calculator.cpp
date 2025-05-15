#include <Calculator.hpp>

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

