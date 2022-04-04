#include <stdio.h>
#include <emscripten/emscripten.h>
#include "api.h"

const int BUFLEN = 100000;
char _result[BUFLEN];
char _version[10];

#ifdef __cplusplus
extern "C" {
#endif

EMSCRIPTEN_KEEPALIVE 
const char* parseMath(const char* str)
	{
	parse(str);
	getResult(_result, BUFLEN);
	return _result;
	}


EMSCRIPTEN_KEEPALIVE 
const char* getMathVersion(void)
	{
	getVersion(_version, 10);
	return _version;
	}

#ifdef __cplusplus
}
#endif


int main() {
    printf("MathParser webassembly module");
	printf("%s\n", getMathVersion());
}

