#pragma once

extern "C" __declspec(dllexport) 
int __cdecl getString(char* buffer, int strlen);

extern "C" __declspec(dllexport)
int __cdecl parse(const char* str);
