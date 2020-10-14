
#include <stdarg.h>
#include "iostream"

void test(const int fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (fmt != NULL) {
            int i = va_arg(args, int);
            std::cout << i << '\n';
            ++fmt;
        }



}