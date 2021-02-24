#include <gtest/gtest.h>
#include <Parser/Parser.hpp>
#include <CommonDataStructure/AST.hpp>
#include <vector>

TEST(ParserTest, Construct)
{
    Parser parser("");
    AST *ast = parser.parse();
    ASSERT_TRUE(nullptr == ast);
}

TEST(ParserTest, matchCharacter)
{
    Parser parser("a");
    AST *ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::CHAR);
    ASSERT_EQ(ast->topNode->token.value, "a");
    ASSERT_EQ(ast->topNode->leftChild, nullptr);
    ASSERT_EQ(ast->topNode->rightChild, nullptr);
}

TEST(ParserTest, matchCharacters)
{
    Parser parser("abc");
    AST *ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::CAT);
    ASSERT_EQ(ast->topNode->token.value, "");

    Node *cat = ast->topNode->leftChild;
    Node *c = ast->topNode->rightChild;
    ASSERT_EQ(cat->token.type, TokenType::CAT);
    ASSERT_EQ(cat->token.value, "");
    ASSERT_EQ(c->token.type, TokenType::CHAR);
    ASSERT_EQ(c->token.value, "c");

    Node *a = cat->leftChild;
    Node *b = cat->rightChild;
    ASSERT_EQ(a->token.type, TokenType::CHAR);
    ASSERT_EQ(a->token.value, "a");
    ASSERT_EQ(b->token.type, TokenType::CHAR);
    ASSERT_EQ(b->token.value, "b");
}

TEST(ParserTest, anyCharExceptNewline)
{
    Parser parser("s.x");
    AST *ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::CAT);
    ASSERT_EQ(ast->topNode->token.value, "");

    Node *cat = ast->topNode->leftChild;
    Node *x = ast->topNode->rightChild;
    ASSERT_EQ(cat->token.type, TokenType::CAT);
    ASSERT_EQ(cat->token.value, "");
    ASSERT_EQ(x->token.type, TokenType::CHAR);
    ASSERT_EQ(x->token.value, "x");

    Node *s = cat->leftChild;
    Node *any = cat->rightChild;
    ASSERT_EQ(s->token.type, TokenType::CHAR);
    ASSERT_EQ(s->token.value, "s");
    ASSERT_EQ(any->token.type, TokenType::ANY_SIGLE_CHAR_EXCEPT_NEWLINE);
    ASSERT_EQ(any->token.value, ".");
}

TEST(ParserTest, CharacterClass)
{
    {
        Parser parser("\\w");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ANY_SINGLE_WORD);
        ASSERT_EQ(ast->topNode->token.value, "\\w");
    }

    {
        Parser parser("\\W");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ANY_SINGLE_NOT_WORD);
        ASSERT_EQ(ast->topNode->token.value, "\\W");
    }

    {
        Parser parser("\\d");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ANY_SINGLE_DIGIT);
        ASSERT_EQ(ast->topNode->token.value, "\\d");
    }

    {
        Parser parser("\\D");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ANY_SINGLE_NOT_DIGIT);
        ASSERT_EQ(ast->topNode->token.value, "\\D");
    }
}

TEST(ParserTest, CharacterRange)
{
    {
        Parser parser("a-z");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::CAT);
        ASSERT_EQ(ast->topNode->token.value, "");

        Node *cat = ast->topNode->leftChild;
        Node *z = ast->topNode->rightChild;
        ASSERT_EQ(cat->token.type, TokenType::CAT);
        ASSERT_EQ(cat->token.value, "");
        ASSERT_EQ(z->token.type, TokenType::CHAR);
        ASSERT_EQ(z->token.value, "z");

        Node *a = cat->leftChild;
        Node *minus = cat->rightChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");
        ASSERT_EQ(minus->token.type, TokenType::CHAR);
        ASSERT_EQ(minus->token.value, "-");
    }

    {
        Parser parser("[a-z]");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::CHARACTER_RANGE);
        ASSERT_EQ(ast->topNode->token.value, "-");

        Node *a = ast->topNode->leftChild;
        Node *z = ast->topNode->rightChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");
        ASSERT_EQ(z->token.type, TokenType::CHAR);
        ASSERT_EQ(z->token.value, "z");
    }
}

TEST(ParserTest, CharacterGroup)
{
    Parser parser("[a0-9\\d]");
    AST *ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::CAT);
    ASSERT_EQ(ast->topNode->token.value, "");

    Node *cat = ast->topNode->leftChild;
    Node *slashD = ast->topNode->rightChild;
    ASSERT_EQ(cat->token.type, TokenType::CAT);
    ASSERT_EQ(cat->token.value, "");
    ASSERT_EQ(slashD->token.type, TokenType::ANY_SINGLE_DIGIT);
    ASSERT_EQ(slashD->token.value, "\\d");

    Node *a = cat->leftChild;
    Node *range = cat->rightChild;
    ASSERT_EQ(a->token.type, TokenType::CHAR);
    ASSERT_EQ(a->token.value, "a");
    ASSERT_EQ(range->token.type, TokenType::CHARACTER_RANGE);
    ASSERT_EQ(range->token.value, "-");

    Node *start = range->leftChild;
    Node *end = range->rightChild;
    ASSERT_EQ(start->token.type, TokenType::CHAR);
    ASSERT_EQ(start->token.value, "0");
    ASSERT_EQ(end->token.type, TokenType::CHAR);
    ASSERT_EQ(end->token.value, "9");
}


TEST(ParserTest, quantifier)
{
    {
        Parser parser("a*");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ZERO_MORE);
        ASSERT_EQ(ast->topNode->token.value, "*");

        Node *a = ast->topNode->leftChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");
    }

    {
        //a+ == aa*
        Parser parser("a+");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::CAT);
        ASSERT_EQ(ast->topNode->token.value, "");

        Node *star = ast->topNode->rightChild;
        ASSERT_EQ(star->token.type, TokenType::ZERO_MORE);
        ASSERT_EQ(star->token.value, "*");

        Node *a = ast->topNode->leftChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");

        a = star->leftChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");
    }

    {
        Parser parser("a?");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ZERO_OR_ONE);
        ASSERT_EQ(ast->topNode->token.value, "?");

        Node *a = ast->topNode->leftChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");
    }
}

TEST(ParserTest, match)
{
    Parser parser("[a0-9\\d]*");
    AST *ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::ZERO_MORE);
    ASSERT_EQ(ast->topNode->token.value, "*");

    Node *matchItem = ast->topNode->leftChild;
    ASSERT_EQ(matchItem->token.type, TokenType::CAT);
    ASSERT_EQ(matchItem->token.value, "");

    Node *cat = matchItem->leftChild;
    Node *slashD = matchItem->rightChild;
    ASSERT_EQ(cat->token.type, TokenType::CAT);
    ASSERT_EQ(cat->token.value, "");
    ASSERT_EQ(slashD->token.type, TokenType::ANY_SINGLE_DIGIT);
    ASSERT_EQ(slashD->token.value, "\\d");

    Node *a = cat->leftChild;
    Node *range = cat->rightChild;
    ASSERT_EQ(a->token.type, TokenType::CHAR);
    ASSERT_EQ(a->token.value, "a");
    ASSERT_EQ(range->token.type, TokenType::CHARACTER_RANGE);
    ASSERT_EQ(range->token.value, "-");

    Node *start = range->leftChild;
    Node *end = range->rightChild;
    ASSERT_EQ(start->token.type, TokenType::CHAR);
    ASSERT_EQ(start->token.value, "0");
    ASSERT_EQ(end->token.type, TokenType::CHAR);
    ASSERT_EQ(end->token.value, "9");
}


TEST(ParserTest, expression)
{
    Parser parser("\\w|\\d|a");
    AST *ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::OR);
    ASSERT_EQ(ast->topNode->token.value, "|");

    Node *subExpression1 = ast->topNode->leftChild;
    Node *subExpression2 = ast->topNode->rightChild;
    ASSERT_EQ(subExpression1->token.type, TokenType::OR);
    ASSERT_EQ(subExpression1->token.value, "|");
    ASSERT_EQ(subExpression2->token.type, TokenType::CHAR);
    ASSERT_EQ(subExpression2->token.value, "a");

    Node *subExpression3 = subExpression1->leftChild;
    Node *subExpression4 = subExpression1->rightChild;
    ASSERT_EQ(subExpression3->token.type, TokenType::ANY_SINGLE_WORD);
    ASSERT_EQ(subExpression3->token.value, "\\w");
    ASSERT_EQ(subExpression4->token.type, TokenType::ANY_SINGLE_DIGIT);
    ASSERT_EQ(subExpression4->token.value, "\\d");
}

TEST(ParserTest, simplegroup)
{
    {
        Parser parser("(a)");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::CHAR);
        ASSERT_EQ(ast->topNode->token.value, "a");
    }

    {
        Parser parser("(a)|(b)");
        AST *ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::OR);
        ASSERT_EQ(ast->topNode->token.value, "|");

        Node *left = ast->topNode->leftChild;
        Node *right = ast->topNode->rightChild;
        ASSERT_EQ(left->token.type, TokenType::CHAR);
        ASSERT_EQ(left->token.value, "a");
        ASSERT_EQ(right->token.type, TokenType::CHAR);
        ASSERT_EQ(right->token.value, "b");
    }
}

TEST(ParserTest, group)
{
    Parser parser("(\\w|\\d|a)*");
    AST *ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::ZERO_MORE);
    ASSERT_EQ(ast->topNode->token.value, "*");

    Node *expr = ast->topNode->leftChild;
    ASSERT_EQ(expr->token.type, TokenType::OR);
    ASSERT_EQ(expr->token.value, "|");

    Node *subExpression1 = expr->leftChild;
    Node *subExpression2 = expr->rightChild;
    ASSERT_EQ(subExpression1->token.type, TokenType::OR);
    ASSERT_EQ(subExpression1->token.value, "|");
    ASSERT_EQ(subExpression2->token.type, TokenType::CHAR);
    ASSERT_EQ(subExpression2->token.value, "a");

    Node *subExpression3 = subExpression1->leftChild;
    Node *subExpression4 = subExpression1->rightChild;
    ASSERT_EQ(subExpression3->token.type, TokenType::ANY_SINGLE_WORD);
    ASSERT_EQ(subExpression3->token.value, "\\w");
    ASSERT_EQ(subExpression4->token.type, TokenType::ANY_SINGLE_DIGIT);
    ASSERT_EQ(subExpression4->token.value, "\\d");
}
