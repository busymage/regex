#ifndef COMMONDATASTRUCTURE_TOKEN_HPP
#define COMMONDATASTRUCTURE_TOKEN_HPP

#include <string>

enum class TokenType{
    ANY_SINGLE_DIGIT,
    ANY_SINGLE_NOT_DIGIT,
    ANY_SINGLE_NOT_WORD,
    ANY_SINGLE_WORD,

    END,
    CHAR,
    INTEGER,
    LETTERS,
    
    STRING_START_ANCHOR,
    STRING_END_ANCHOR,

    ZERO_MORE,
    ONE_MORE,
    ZERO_OR_ONE,
    CHARACTER_RANGE,

    LEFT_PAREN,
    RIGHT_PAREN,
    OR,
    ANY_SIGLE_CHAR_EXCEPT_NEWLINE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    SLASH,
    COMMA,

    CAT,
    UNKNOWN
};

struct Token{
    TokenType type;
    std::string value;
};

#endif