#pragma once

#ifdef __WASM__
    #define C_IMP_EXP
    #define C_DECL
#elif defined MATHPARSERDLL_EXPORTS
    #define C_IMP_EXP extern "C" __declspec(dllexport)
    #define C_DECL __cdecl
#else
    #define C_IMP_EXP extern "C" __declspec(dllimport)
    #define C_DECL __cdecl
#endif

//Parses the string and returns the length of resulting json.
C_IMP_EXP void C_DECL setSource(const char* scriptId, const char* text);

C_IMP_EXP int C_DECL parse(const char* str);

C_IMP_EXP int C_DECL getResultLen();

C_IMP_EXP void C_DECL getResult(char* buffer, int buflen);

C_IMP_EXP void C_DECL getVersion(char* buffer, int buflen);
