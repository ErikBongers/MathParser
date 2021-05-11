#include "concat.h"

char _message[1001];
void concatStrings(const std::string& s1, const std::string& s2)
{
    std::string concat;
    concat += s1 + s2;
    std::strcpy (_message, concat.c_str()); //TODO: limit to buffer length
}

