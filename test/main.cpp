#include <iostream>
#include <MatrixGeneric.hpp>

template<typename T>
void print_matrix(const MatrixGeneric<T> &g)
{
    for (uint32_t i = 0; i < g.height(); ++i)
    {
        for (uint32_t j = 0; j < g.width(); ++j)
        {
            std::cout << g.get(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
}
