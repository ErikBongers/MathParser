#pragma once

struct SourcePos
    {
    unsigned int line = 0;
    unsigned int linePos = 0;
    unsigned long cursorPos = 0;
    char sourceIndex;
    SourcePos operator-(int n) const { return {line, linePos - n, cursorPos - n, sourceIndex}; }
    };
