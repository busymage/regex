#include <ctype.h>
#include <memory>
#include <Scanner/Scanner.hpp>
#include <string>
#include <vector>

bool isWhiteSpace(char c)
{
    return ' ' == c || '\n' == c || '\t' == c; 
}

struct Scanner::Impl{
    std::string source;
    size_t index;

    char currentChar()
    {
        return source[index];
    }

    char currentCharNAdvance()
    {
        if(index == source.length()){
            return source[index];
        }
        return source[index++];
    }
};

Scanner::Scanner(const std::string &input)
    :impl_(new Impl)
{
    impl_->source = input;
    impl_->index = 0;
}

Scanner::~Scanner() = default;

Token Scanner::getNextToken()
{
    Token token;
    if (impl_->source.length() == 0){
        token.type = TokenType::END;
        return token;
    }

    if(impl_->index >= impl_->source.length()){
        token.type = TokenType::END;
        return token;
    }
    
    char c = impl_->currentCharNAdvance();

    if('\\' == c){
        char c = impl_->currentChar();
        switch (c)
        {
        case 'w':
            token.type = TokenType::ANY_SINGLE_WORD;
            token.value = "\\w";
            impl_->index++;
            return token;
        
        case 'W':
            token.type = TokenType::ANY_SINGLE_NOT_WORD;
            token.value = "\\W";
            impl_->index++;
            return token;
        
        case 'd':
            token.type = TokenType::ANY_SINGLE_DIGIT;
            token.value = "\\d";
            impl_->index++;
            return token;
        
        case 'D':
            token.type = TokenType::ANY_SINGLE_NOT_DIGIT;
            token.value = "\\D";
            impl_->index++;
            return token;
        default:
            token.type = TokenType::SLASH;
            token.value = "\\";
            return token;
        }
    }

    else if('^' == c){
        token.type = TokenType::STRING_START_ANCHOR;
    }

    else if('$' == c){
        token.type = TokenType::STRING_END_ANCHOR;
    }
    
    else if('|' == c){
        token.type = TokenType::OR;
    }

    else if('(' == c){
        token.type = TokenType::LEFT_PAREN;
    }

    else if(')' == c){
        token.type = TokenType::RIGHT_PAREN;
    }

    else if('[' == c){
        token.type = TokenType::LEFT_BRACKET;
    }

    else if(']' == c){
        token.type = TokenType::RIGHT_BRACKET;
    }

    else if('{' == c){
        token.type = TokenType::LEFT_BRACE;
    }

    else if('}' == c){
        token.type = TokenType::RIGHT_BRACE;
    }

    else if(',' == c){
        token.type = TokenType::COMMA;
    }

    else if('.' == c){
        token.type = TokenType::ANY_SIGLE_CHAR_EXCEPT_NEWLINE;
    }

    else if('+' == c){
        token.type = TokenType::ONE_MORE;
    }

    else if('-' == c){
        token.type = TokenType::CHARACTER_RANGE;
    }

    else if('*' == c){
        token.type = TokenType::ZERO_MORE;
    }
    
    else if('?' == c){
        token.type = TokenType::ZERO_OR_ONE;
    }

    else if(0x09 == c ||
            0x0a == c ||
            0x0d == c ||
            (0x20 <= c && 0x7ff >= c)
    ){
        token.type = TokenType::CHAR;
    }

    else{
        token.type = TokenType::UNKNOWN;
    }

    token.value = c;
    return token;
}

std::vector<Token> Scanner::getNextTokens(int num)
{
    int totaltoken = 0;
    std::vector<Token> tokens;
    while(totaltoken < num){
        Token t = getNextToken();
        tokens.push_back(t);
        if(TokenType::END == t.type){
            break;
        }
        totaltoken++;
    }
    return tokens;
}

Token Scanner::peekNextToken()
{
    size_t oldIndex = impl_->index;
    Token t = getNextToken();
    impl_->index = oldIndex;
    return t;
}

std::vector<Token> Scanner::peekNextTokens(int num)
{
    size_t oldIndex = impl_->index;
    std::vector<Token> tokens = getNextTokens(num);
    impl_->index = oldIndex;
    return tokens;
}