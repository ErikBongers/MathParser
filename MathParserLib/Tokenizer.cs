using System;

namespace MathParserLib
    {

    public enum TokenType
        {
        BRAC_OPEN,
        BRAC_CLOSE,
        CURL_OPEN,
        CURL_CLOSE,
        PAR_OPEN,
        PAR_CLOSE,
        PLUS,
        MIN,
        DIV,
        MULT,
        EQ,
        NUMBER,
        POWER,
        ID,
        RAD,
        DEG,
        SEMI_COLON,

        UNKNOWN,
        EOF
        }

    public class Token
        {
        public TokenType Type;
        public string StringValue;
        public double NumberValue;
        public Token(TokenType type, char c)
            {
            this.Type = type;
            this.StringValue = c.ToString();
            }
        public Token(TokenType type, double d)
            {
            this.Type = type;
            this.NumberValue = d;
            }
        public Token(TokenType type, string str)
            {
            this.Type = type;
            this.StringValue = str;
            }
        public Token(TokenType type) : this(type, ' ') { }
        public override string ToString()
            {
            if (Type == TokenType.NUMBER)
                return NumberValue.ToString();
            else if (Type == TokenType.ID)
                return StringValue;
            else
                return Type.ToString();
            }
        }

    public class Tokenizer
        {
        private string stream;
        int pos = 0; //pos at which to read next token.
        public Tokenizer(string stream)
            {
            this.stream = stream;
            }

        Token peekedToken = null;
        public Token peek()
            {
            if (peekedToken != null)
                return peekedToken;
            peekedToken = next();
            return peekedToken;
            }

        public Token next()
            {
            if (peekedToken != null)
                {
                Token t = peekedToken;
                peekedToken = null;
                return t;
                }
            if (pos >= stream.Length)
                return new Token(TokenType.EOF);

            //skip whitespace
            while (pos < stream.Length)
                {
                if (stream[pos] != ' ' && stream[pos] != '\n' && stream[pos] != '\r')
                    break;
                pos++;
                }

            if (pos >= stream.Length)
                return new Token(TokenType.EOF);

            char c = stream[pos];
            pos++;
            switch (c)
                {
                case '{':
                    return new Token(TokenType.CURL_OPEN, c);
                case '}':
                    return new Token(TokenType.CURL_CLOSE, c);
                case '(':
                    return new Token(TokenType.PAR_OPEN, c);
                case ')':
                    return new Token(TokenType.PAR_CLOSE, c);
                case '[':
                    return new Token(TokenType.BRAC_OPEN, c);
                case ']':
                    return new Token(TokenType.BRAC_CLOSE, c);

                case '+':
                    return new Token(TokenType.PLUS, c);
                case '-':
                    return new Token(TokenType.MIN, c);
                case '*':
                    return new Token(TokenType.MULT, c);
                case '/':
                    return new Token(TokenType.DIV, c);
                case '^':
                    return new Token(TokenType.POWER, c);
                case '=':
                    return new Token(TokenType.EQ, c);
                case ';':
                    return new Token(TokenType.SEMI_COLON, c);
                case char n when n >= '0' && n <= '9':
                case '.':
                    return parseNumber(c);
                case char l when l >= 'a' && l <= 'z':
                case char z when z >= 'A' && z <= 'Z':
                    return parseId(c);
                default:
                    return new Token(TokenType.UNKNOWN, c);
                }
            }

        private Token parseId(char c)
            {
            string word = "";

            word += c.ToString();

            while (pos < stream.Length)
                {
                c = stream[pos];
                pos++;
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
                    {
                    word += c;
                    }
                else
                    {
                    pos--;
                    break;
                    }
                }
            if (word.ToLower() == "deg")
                return new Token(TokenType.DEG, word);
            else if (word.ToLower() == "rad")
                return new Token(TokenType.RAD, word);
            else
                return new Token(TokenType.ID, word);
            }

        private Token parseNumber(char c)
            {
            //we already have the first digit
            double d = 0;
            double decimalDivider = 1;
            if (c == '.')
                decimalDivider = 10;
            else
                d = c - '0';

            while (pos < stream.Length)
                {
                c = stream[pos];
                pos++;
                if (c >= '0' && c <= '9')
                    {
                    if (decimalDivider == 1)
                        d = d * 10 + (c - '0');
                    else
                        {
                        d += (c - '0') / decimalDivider;
                        decimalDivider *= 10;
                        }
                    }
                else if (c == '.')
                    {
                    decimalDivider = 10;
                    }
                else
                    {
                    pos--;
                    break;
                    }
                }

            return new Token(TokenType.NUMBER, d);
            }
        }
    }
