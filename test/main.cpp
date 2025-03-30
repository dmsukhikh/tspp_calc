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

    MatrixGeneric a = {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, {5, 6, 1, 2, 3}};
    MatrixGeneric b = {{0, 2, 1, 4, 5}, {-10, 2, 30, 4, 25}, {5, -6, 1, 2, 3}};

    print_matrix(a + b);
}
