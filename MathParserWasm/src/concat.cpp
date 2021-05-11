#include "concat.h"
#include "json.h"

char _message[1001];
void concatStrings(const std::string& s1, const std::string& s2)
{
    std::string concat;
    concat += s1 + s2;
    concat = escape_json(concat);
    std::strcpy (_message, concat.c_str()); //TODO: limit to buffer length
}

