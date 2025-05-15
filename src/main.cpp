#include <Calculator.hpp>

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout << "[calc] usage: calc [file] [-f|-d]" << std::endl;
        return -1;
    }

    try
    {
        if (std::string(argv[2]) == "-f")
        {
            mainRoutine<float>(argv);
        }
        else if (std::string(argv[2]) == "-d")
        {
            mainRoutine<double>(argv);
        }
        else
        {
            std::cout << "Invalid flag: " << argv[2]
                      << ". Supported flags: -f, -d" << std::endl;
            return -1;
        }
    }
    catch (...)
    {
        return -1;
    }

    return 0;
}

