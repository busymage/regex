#include <gtest/gtest.h>
#include <Scanner/Scanner.hpp>
#include <vector>

TEST(ScannerTest, Construct)
{
    Scanner scanner("");
    Token token = scanner.getNextToken();
    ASSERT_EQ(TokenType::END, token.type);
}

TEST(ScannerTest, eofAlwaysGetSameToken)
{
    Scanner scanner("");
    Token token1 = scanner.getNextToken();
    Token token2 = scanner.getNextToken();
    ASSERT_EQ(TokenType::END, token1.type);
    ASSERT_EQ(TokenType::END, token2.type);
    ASSERT_EQ(token1.type, token2.type);
}

TEST(ScannerTest, charToken)
{
    Scanner scanner("12asd\r\n");
    Token token1 = scanner.getNextToken();
    ASSERT_EQ(TokenType::CHAR, token1.type);
    ASSERT_EQ("1", token1.value);

    Token token2 = scanner.getNextToken();
    ASSERT_EQ(TokenType::CHAR, token2.type);
    ASSERT_EQ("2", token2.value);

    Token token3 = scanner.getNextToken();
    ASSERT_EQ(TokenType::CHAR, token3.type);
    ASSERT_EQ("a", token3.value);

    Token token4 = scanner.getNextToken();
    ASSERT_EQ(TokenType::CHAR, token4.type);
    ASSERT_EQ("s", token4.value);

    Token token5 = scanner.getNextToken();
    ASSERT_EQ(TokenType::CHAR, token5.type);
    ASSERT_EQ("d", token5.value);

    Token token6 = scanner.getNextToken();
    ASSERT_EQ(TokenType::CHAR, token6.type);
    ASSERT_EQ("\r", token6.value);

    Token token7 = scanner.getNextToken();
    ASSERT_EQ(TokenType::CHAR, token7.type);
    ASSERT_EQ("\n", token7.value);
}

TEST(ScannerTest, anchors)
{
    Scanner scanner("^$");
    Token token1 = scanner.getNextToken();
    ASSERT_EQ(TokenType::STRING_START_ANCHOR, token1.type);
    ASSERT_EQ("^", token1.value);

    Token token2 = scanner.getNextToken();
    ASSERT_EQ(TokenType::STRING_END_ANCHOR, token2.type);
    ASSERT_EQ("$", token2.value);

    Token token6 = scanner.getNextToken();
    ASSERT_EQ(TokenType::END, token6.type);
}

TEST(ScannerTest, characterClass)
{
    Scanner scanner("\\w\\W\\d\\D");
    Token token1 = scanner.getNextToken();
    ASSERT_EQ(TokenType::ANY_SINGLE_WORD, token1.type);
    ASSERT_EQ("\\w", token1.value);

    Token token2 = scanner.getNextToken();
    ASSERT_EQ(TokenType::ANY_SINGLE_NOT_WORD, token2.type);
    ASSERT_EQ("\\W", token2.value);

    Token token3 = scanner.getNextToken();
    ASSERT_EQ(TokenType::ANY_SINGLE_DIGIT, token3.type);
    ASSERT_EQ("\\d", token3.value);

    Token token4 = scanner.getNextToken();
    ASSERT_EQ(TokenType::ANY_SINGLE_NOT_DIGIT, token4.type);
    ASSERT_EQ("\\D", token4.value);

    Token token6 = scanner.getNextToken();
    ASSERT_EQ(TokenType::END, token6.type);
}

TEST(ScannerTest, quantifier)
{
    Scanner scanner("*+?-");
    Token token1 = scanner.getNextToken();
    ASSERT_EQ(TokenType::ZERO_MORE, token1.type);
    ASSERT_EQ("*", token1.value);

    Token token2 = scanner.getNextToken();
    ASSERT_EQ(TokenType::ONE_MORE, token2.type);
    ASSERT_EQ("+", token2.value);

    Token token3 = scanner.getNextToken();
    ASSERT_EQ(TokenType::ZERO_OR_ONE, token3.type);
    ASSERT_EQ("?", token3.value);

    Token token4 = scanner.getNextToken();
    ASSERT_EQ(TokenType::CHARACTER_RANGE, token4.type);
    ASSERT_EQ("-", token4.value);

    Token token6 = scanner.getNextToken();
    ASSERT_EQ(TokenType::END, token6.type);
}

TEST(ScannerTest, others)
{
    Scanner scanner("|().[]{}\\,");
    Token token1 = scanner.getNextToken();
    ASSERT_EQ(TokenType::OR, token1.type);
    ASSERT_EQ("|", token1.value);

    Token token2 = scanner.getNextToken();
    ASSERT_EQ(TokenType::LEFT_PAREN, token2.type);
    ASSERT_EQ("(", token2.value);

    Token token3 = scanner.getNextToken();
    ASSERT_EQ(TokenType::RIGHT_PAREN, token3.type);
    ASSERT_EQ(")", token3.value);

    Token token5 = scanner.getNextToken();
    ASSERT_EQ(TokenType::ANY_SIGLE_CHAR_EXCEPT_NEWLINE , token5.type);
    ASSERT_EQ(".", token5.value);

    Token token6 = scanner.getNextToken();
    ASSERT_EQ(TokenType::LEFT_BRACKET, token6.type);
    ASSERT_EQ("[", token6.value);

    Token token7 = scanner.getNextToken();
    ASSERT_EQ(TokenType::RIGHT_BRACKET, token7.type);
    ASSERT_EQ("]", token7.value);

    Token token8 = scanner.getNextToken();
    ASSERT_EQ(TokenType::LEFT_BRACE, token8.type);
    ASSERT_EQ("{", token8.value);

    Token token9 = scanner.getNextToken();
    ASSERT_EQ(TokenType::RIGHT_BRACE, token9.type);
    ASSERT_EQ("}", token9.value);

    Token token10 = scanner.getNextToken();
    ASSERT_EQ(TokenType::SLASH, token10.type);
    ASSERT_EQ("\\", token10.value);

    Token token11 = scanner.getNextToken();
    ASSERT_EQ(TokenType::COMMA, token11.type);
    ASSERT_EQ(",", token11.value);

    Token token20 = scanner.getNextToken();
    ASSERT_EQ(TokenType::END, token20.type);
}

TEST(ScannerTest, getMultiToken)
{
    std::vector<Token> testVector = {
        {TokenType::STRING_START_ANCHOR, "^"},
        {TokenType::CHAR, "t"},
        {TokenType::CHAR, "e"},
        {TokenType::STRING_END_ANCHOR, "$"},
        {TokenType::END, ""}
    };

    Scanner scanner("^te$");
    std::vector<Token> tokens = scanner.getNextTokens(testVector.size());
    for (size_t i = 0; i < testVector.size(); i++)
    {
        ASSERT_EQ(tokens[i].type, testVector[i].type);
        ASSERT_EQ(tokens[i].value, testVector[i].value);
    }
}

TEST(ScannerTest, getTooMuchOrTooLessTokens)
{
    {
        std::vector<Token> testVector = {
            {TokenType::STRING_START_ANCHOR, "^"},
            {TokenType::CHAR, "a"},
            {TokenType::ONE_MORE, "+"},
            {TokenType::END, ""}
        };

        Scanner scanner("^a+");
        std::vector<Token> tokens = scanner.getNextTokens(7);
        ASSERT_EQ(tokens.size(),testVector.size());
        for (size_t i = 0; i < testVector.size(); i++)
        {
            ASSERT_EQ(tokens[i].type, testVector[i].type);
            ASSERT_EQ(tokens[i].value, testVector[i].value);
        }
    }

    {
        std::vector<Token> testVector = {
            {TokenType::STRING_START_ANCHOR, "^"},
            {TokenType::CHAR, "a"},
        };

        Scanner scanner("^a+");
        std::vector<Token> tokens = scanner.getNextTokens(2);
        ASSERT_EQ(tokens.size(),testVector.size());
        for (size_t i = 0; i < testVector.size(); i++)
        {
            ASSERT_EQ(tokens[i].type, testVector[i].type);
            ASSERT_EQ(tokens[i].value, testVector[i].value);
        }
    }
}

TEST(ScannerTest, peekToken)
{
    Scanner scanner("^a+");
    Token token = scanner.peekNextToken();
    Token token1 = scanner.peekNextToken();
    ASSERT_EQ(token.type, token1.type);
    ASSERT_EQ(token.value, token1.value);
    ASSERT_EQ(token.value, "^");

    scanner.getNextToken();
    Token token2 = scanner.peekNextToken();
    Token token3 = scanner.peekNextToken();
    ASSERT_EQ(token2.type, token3.type);
    ASSERT_EQ(token2.value, token3.value);
    ASSERT_EQ(token2.value, "a");

    scanner.getNextToken();
    Token token4 = scanner.peekNextToken();
    Token token5 = scanner.peekNextToken();
    ASSERT_EQ(token4.type, token5.type);
    ASSERT_EQ(token4.value, token5.value);
    ASSERT_EQ(token4.value, "+");
}

TEST(ScannerTest, peekMultiToken)
{
    std::vector<Token> testVector = {
        {TokenType::STRING_START_ANCHOR, "^"},
        {TokenType::CHAR, "T"},
        {TokenType::CHAR, "E"},
        {TokenType::STRING_END_ANCHOR, "$"},
        {TokenType::END, ""}
    };

    Scanner scanner("^TE$");
    std::vector<Token> tokens = scanner.peekNextTokens(testVector.size());
    for (size_t i = 0; i < testVector.size(); i++)
    {
        ASSERT_EQ(tokens[i].type, testVector[i].type);
        ASSERT_EQ(tokens[i].value, testVector[i].value);
    }

    scanner.getNextToken();
    tokens = scanner.peekNextTokens(testVector.size());
    ASSERT_EQ(tokens.size(), testVector.size() -1);
    for (size_t i = 1; i < testVector.size(); i++)
    {
        ASSERT_EQ(tokens[i-1].type, testVector[i].type);
        ASSERT_EQ(tokens[i-1].value, testVector[i].value);
    }
}