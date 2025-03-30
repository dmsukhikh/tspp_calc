#ifndef __MATRIX_EXCEPT
#define __MATRIX_EXCEPT

#include <exception>

class basic_matrix_exception : public std::exception
{
    const char *_msg;
public:
    basic_matrix_exception(const char *msg) : _msg(msg) {} 
    const char * what() const noexcept override
    {
        return _msg;
    }
};

class matrix_initialization_error : public basic_matrix_exception 
{
public:
    matrix_initialization_error(const char *msg) :
        basic_matrix_exception(msg) {}
};

class matrix_bad_access : public basic_matrix_exception 
{
public:
    matrix_bad_access(const char *msg) :
        basic_matrix_exception(msg) {}
};

class matrix_bad_det: public basic_matrix_exception 
{
public:
    matrix_bad_det(const char *msg) :
        basic_matrix_exception(msg) {}
};

class matrix_bad_pow: public basic_matrix_exception 
{
public:
    matrix_bad_pow(const char *msg) :
        basic_matrix_exception(msg) {}
};

class matrix_bad_inverse: public basic_matrix_exception 
{
public:
    matrix_bad_inverse(const char *msg) :
        basic_matrix_exception(msg) {}
};

#endif
