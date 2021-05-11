#pragma once

#ifdef __WASM__
    #define C_EXPORT
    #define C_DECL
#else
    #define C_EXPORT extern "C" __declspec(dllexport)
    #define C_DECL __cdecl
#endif

C_EXPORT int C_DECL parse(const char* str);

C_EXPORT int C_DECL getResultLen();

C_EXPORT void C_DECL getResult(char* buffer, int strlen);
