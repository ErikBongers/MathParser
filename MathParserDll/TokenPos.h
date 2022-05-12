#pragma once

struct TokenPos
    {
    int line = 0;
    int linePos = 0;
    int cursorPos = 0;
    TokenPos offset (int n) const
        {
        return TokenPos(line, linePos-n, cursorPos-n);
        }
    };
