#pragma once

struct TokenPos
    {
    int line = 0;
    int linePos = 0;
    int cursorPos = 0;
    TokenPos operator-(int n) const { return {line, linePos - n, cursorPos - n}; }
    };
