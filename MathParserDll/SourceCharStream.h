#pragma once
#include "SourcePos.h"
#include "Source.h"

struct StreamState
    {
    SourcePos pos;
    bool newLineStarted = true;
    };

class SourceCharStream
    {
    private:
        size_t size = -1;
    
    public:
        const Source& source;
        StreamState state;

        SourceCharStream(const Source& source) : source(source)
            {
            size = source.text.size(); 
            }

        inline constexpr const char* c_str() { return source.text.c_str(); }

        inline size_t getSize() { return size;}

        std::string getText(unsigned int start, unsigned end) { return std::string(&source.text[start], &source.text[end]); }

        char peekChar()
            {
            if(state.pos.cursorPos >= size)
                return 0; //EOF
            return source.text[state.pos.cursorPos];
            }

        char peekSecondChar()
            {
            if((state.pos.cursorPos+1) >= size)
                return 0; //EOF
            return source.text[state.pos.cursorPos+1];
            }

        char nextChar()
            {
            if(state.pos.cursorPos >= size)
                return 0; //EOF
            if(source.text[state.pos.cursorPos] == '\n')
                {
                state.pos.line++;
                state.pos.linePos = 0;
                state.newLineStarted = true;
                }
            else
                state.pos.linePos++;
            return source.text[state.pos.cursorPos++];
            }

        bool match(char c)
            {
            if (peekChar() == c)
                {
                nextChar();
                return true;
                }
            return false;
            }


    };