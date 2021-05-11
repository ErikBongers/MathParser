#include <stdio.h>
#include <emscripten/emscripten.h>
#include "concat.h"

int main() {
    printf("Hullo World\n");
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



#ifdef __cplusplus
}
#endif