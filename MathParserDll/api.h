#pragma once

extern "C" __declspec(dllexport)
int __cdecl parse(const char* str);

extern "C" __declspec(dllexport)
int __cdecl getResultLen();

extern "C" __declspec(dllexport)
void __cdecl getResult(char* buffer, int strlen);
