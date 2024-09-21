#include "_printf_.h"


int printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vprintf_(format, args);
    va_end(args);
    return ret;
}

int sprintf(char* buffer, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vsnprintf_(buffer, ~0, format, args);
    va_end(args);
    return ret;
}

int snprintf(char* buffer, size_t count, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vsnprintf_(buffer, count, format, args);
    va_end(args);
    return ret;
}

int vsnprintf(char* buffer, size_t count, const char* format, va_list va)
{
    return vsnprintf_(buffer, count, format, va);
}

int vprintf(const char* format, va_list va)
{
    return vprintf_(format, va);
}
