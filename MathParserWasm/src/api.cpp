#include <stdio.h>
#include <emscripten/emscripten.h>
#include "api.h"

char _result[10000];

int main() {
    printf("MathParser webassembly module 0.11\n");
}

#ifdef __cplusplus
extern "C" {
#endif

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