#include <stdio.h>
#include <emscripten/emscripten.h>
#include "concat.h"
#include "api.h"

char _result[10000];

int main() {
    printf("MathParser webassembly module 0.1\n");
}

#ifdef __cplusplus
extern "C" {
#endif

EMSCRIPTEN_KEEPALIVE 
const char* getMessage(const char* text) 
    {
    printf("inside getMessage()\n");
    concatStrings("The message is : ", text);
    return _message;
    }

EMSCRIPTEN_KEEPALIVE 
const char* parseMath(const char* str)
	{
	parse(str);
	getResult(_result, 10000);
	return _result;
	}

#ifdef __cplusplus
}
#endif