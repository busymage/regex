#include <gtest/gtest.h>
#include <Parser/Parser.hpp>
#include <Parser/AST.hpp>
#include <vector>

TEST(ParserTest, Construct)
{
    Parser parser("");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_TRUE(nullptr == ast);
}

TEST(ParserTest, matchCharacter)
{
    Parser parser("a");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::CHAR);
    ASSERT_EQ(ast->topNode->token.value, "a");
    ASSERT_EQ(ast->topNode->leftChild, nullptr);
    ASSERT_EQ(ast->topNode->rightChild, nullptr);
}

TEST(ParserTest, matchCharacters)
{
    Parser parser("abc");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::CAT);
    ASSERT_EQ(ast->topNode->token.value, "");

    ASTNode*cat = ast->topNode->leftChild;
    ASTNode*c = ast->topNode->rightChild;
    ASSERT_EQ(cat->token.type, TokenType::CAT);
    ASSERT_EQ(cat->token.value, "");
    ASSERT_EQ(c->token.type, TokenType::CHAR);
    ASSERT_EQ(c->token.value, "c");

    ASTNode*a = cat->leftChild;
    ASTNode*b = cat->rightChild;
    ASSERT_EQ(a->token.type, TokenType::CHAR);
    ASSERT_EQ(a->token.value, "a");
    ASSERT_EQ(b->token.type, TokenType::CHAR);
    ASSERT_EQ(b->token.value, "b");
}

TEST(ParserTest, anyCharExceptNewline)
{
    Parser parser("s.x");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::CAT);
    ASSERT_EQ(ast->topNode->token.value, "");

    ASTNode*cat = ast->topNode->leftChild;
    ASTNode*x = ast->topNode->rightChild;
    ASSERT_EQ(cat->token.type, TokenType::CAT);
    ASSERT_EQ(cat->token.value, "");
    ASSERT_EQ(x->token.type, TokenType::CHAR);
    ASSERT_EQ(x->token.value, "x");

    ASTNode*s = cat->leftChild;
    ASTNode*any = cat->rightChild;
    ASSERT_EQ(s->token.type, TokenType::CHAR);
    ASSERT_EQ(s->token.value, "s");
    ASSERT_EQ(any->token.type, TokenType::ANY_SIGLE_CHAR_EXCEPT_NEWLINE);
    ASSERT_EQ(any->token.value, ".");
}

TEST(ParserTest, CharacterClass)
{
    {
        Parser parser("\\w");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ANY_SINGLE_WORD);
        ASSERT_EQ(ast->topNode->token.value, "\\w");
    }

    {
        Parser parser("\\W");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ANY_SINGLE_NOT_WORD);
        ASSERT_EQ(ast->topNode->token.value, "\\W");
    }

    {
        Parser parser("\\d");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ANY_SINGLE_DIGIT);
        ASSERT_EQ(ast->topNode->token.value, "\\d");
    }

    {
        Parser parser("\\D");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ANY_SINGLE_NOT_DIGIT);
        ASSERT_EQ(ast->topNode->token.value, "\\D");
    }
}

TEST(ParserTest, CharacterRange)
{
    {
        Parser parser("a-z");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::CAT);
        ASSERT_EQ(ast->topNode->token.value, "");

        ASTNode*cat = ast->topNode->leftChild;
        ASTNode*z = ast->topNode->rightChild;
        ASSERT_EQ(cat->token.type, TokenType::CAT);
        ASSERT_EQ(cat->token.value, "");
        ASSERT_EQ(z->token.type, TokenType::CHAR);
        ASSERT_EQ(z->token.value, "z");

        ASTNode*a = cat->leftChild;
        ASTNode*minus = cat->rightChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");
        ASSERT_EQ(minus->token.type, TokenType::CHAR);
        ASSERT_EQ(minus->token.value, "-");
    }

    {
        Parser parser("[a-z]");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::CHARACTER_RANGE);
        ASSERT_EQ(ast->topNode->token.value, "-");

        ASTNode*a = ast->topNode->leftChild;
        ASTNode*z = ast->topNode->rightChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");
        ASSERT_EQ(z->token.type, TokenType::CHAR);
        ASSERT_EQ(z->token.value, "z");
    }
}

TEST(ParserTest, CharacterGroup)
{
    Parser parser("[a0-9\\d]");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::OR);
    ASSERT_EQ(ast->topNode->token.value, "|");

    ASTNode*cat = ast->topNode->leftChild;
    ASTNode*slashD = ast->topNode->rightChild;
    ASSERT_EQ(cat->token.type, TokenType::OR);
    ASSERT_EQ(cat->token.value, "|");
    ASSERT_EQ(slashD->token.type, TokenType::ANY_SINGLE_DIGIT);
    ASSERT_EQ(slashD->token.value, "\\d");

    ASTNode*a = cat->leftChild;
    ASTNode*range = cat->rightChild;
    ASSERT_EQ(a->token.type, TokenType::CHAR);
    ASSERT_EQ(a->token.value, "a");
    ASSERT_EQ(range->token.type, TokenType::CHARACTER_RANGE);
    ASSERT_EQ(range->token.value, "-");

    ASTNode*start = range->leftChild;
    ASTNode*end = range->rightChild;
    ASSERT_EQ(start->token.type, TokenType::CHAR);
    ASSERT_EQ(start->token.value, "0");
    ASSERT_EQ(end->token.type, TokenType::CHAR);
    ASSERT_EQ(end->token.value, "9");
}


TEST(ParserTest, quantifier)
{
    {
        Parser parser("a*");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::ZERO_MORE);
        ASSERT_EQ(ast->topNode->token.value, "*");

        ASTNode*a = ast->topNode->leftChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");
    }

    {
        //a+ == aa*
        Parser parser("a+");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::CAT);
        ASSERT_EQ(ast->topNode->token.value, "");

        ASTNode*star = ast->topNode->rightChild;
        ASSERT_EQ(star->token.type, TokenType::ZERO_MORE);
        ASSERT_EQ(star->token.value, "*");

        ASTNode*a = ast->topNode->leftChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");

        a = star->leftChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");
    }

    {
        Parser parser("a?");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::RANGE_QUANTIFER);
        ASSERT_EQ(ast->topNode->token.value, "0,1");

        ASTNode*a = ast->topNode->leftChild;
        ASSERT_EQ(a->token.type, TokenType::CHAR);
        ASSERT_EQ(a->token.value, "a");
    }
}

TEST(ParserTest, match)
{
    Parser parser("[a0-9\\d]*");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::ZERO_MORE);
    ASSERT_EQ(ast->topNode->token.value, "*");

    ASTNode*matchItem = ast->topNode->leftChild;
    ASSERT_EQ(matchItem->token.type, TokenType::OR);
    ASSERT_EQ(matchItem->token.value, "|");

    ASTNode*cat = matchItem->leftChild;
    ASTNode*slashD = matchItem->rightChild;
    ASSERT_EQ(cat->token.type, TokenType::OR);
    ASSERT_EQ(cat->token.value, "|");
    ASSERT_EQ(slashD->token.type, TokenType::ANY_SINGLE_DIGIT);
    ASSERT_EQ(slashD->token.value, "\\d");

    ASTNode*a = cat->leftChild;
    ASTNode*range = cat->rightChild;
    ASSERT_EQ(a->token.type, TokenType::CHAR);
    ASSERT_EQ(a->token.value, "a");
    ASSERT_EQ(range->token.type, TokenType::CHARACTER_RANGE);
    ASSERT_EQ(range->token.value, "-");

    ASTNode*start = range->leftChild;
    ASTNode*end = range->rightChild;
    ASSERT_EQ(start->token.type, TokenType::CHAR);
    ASSERT_EQ(start->token.value, "0");
    ASSERT_EQ(end->token.type, TokenType::CHAR);
    ASSERT_EQ(end->token.value, "9");
}


TEST(ParserTest, expression)
{
    Parser parser("\\w|\\d|a");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::OR);
    ASSERT_EQ(ast->topNode->token.value, "|");

    ASTNode*subExpression1 = ast->topNode->leftChild;
    ASTNode*subExpression2 = ast->topNode->rightChild;
    ASSERT_EQ(subExpression1->token.type, TokenType::OR);
    ASSERT_EQ(subExpression1->token.value, "|");
    ASSERT_EQ(subExpression2->token.type, TokenType::CHAR);
    ASSERT_EQ(subExpression2->token.value, "a");

    ASTNode*subExpression3 = subExpression1->leftChild;
    ASTNode*subExpression4 = subExpression1->rightChild;
    ASSERT_EQ(subExpression3->token.type, TokenType::ANY_SINGLE_WORD);
    ASSERT_EQ(subExpression3->token.value, "\\w");
    ASSERT_EQ(subExpression4->token.type, TokenType::ANY_SINGLE_DIGIT);
    ASSERT_EQ(subExpression4->token.value, "\\d");
}

TEST(ParserTest, simplegroup)
{
    {
        Parser parser("(a)");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::CHAR);
        ASSERT_EQ(ast->topNode->token.value, "a");
    }

    {
        Parser parser("(a)|(b)");
        std::shared_ptr<AST>ast = parser.parse();
        ASSERT_EQ(ast->topNode->token.type, TokenType::OR);
        ASSERT_EQ(ast->topNode->token.value, "|");

        ASTNode*left = ast->topNode->leftChild;
        ASTNode*right = ast->topNode->rightChild;
        ASSERT_EQ(left->token.type, TokenType::CHAR);
        ASSERT_EQ(left->token.value, "a");
        ASSERT_EQ(right->token.type, TokenType::CHAR);
        ASSERT_EQ(right->token.value, "b");
    }
}

TEST(ParserTest, group)
{
    Parser parser("(\\w|\\d|a)*");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::ZERO_MORE);
    ASSERT_EQ(ast->topNode->token.value, "*");

    ASTNode*expr = ast->topNode->leftChild;
    ASSERT_EQ(expr->token.type, TokenType::OR);
    ASSERT_EQ(expr->token.value, "|");

    ASTNode*subExpression1 = expr->leftChild;
    ASTNode*subExpression2 = expr->rightChild;
    ASSERT_EQ(subExpression1->token.type, TokenType::OR);
    ASSERT_EQ(subExpression1->token.value, "|");
    ASSERT_EQ(subExpression2->token.type, TokenType::CHAR);
    ASSERT_EQ(subExpression2->token.value, "a");

    ASTNode*subExpression3 = subExpression1->leftChild;
    ASTNode*subExpression4 = subExpression1->rightChild;
    ASSERT_EQ(subExpression3->token.type, TokenType::ANY_SINGLE_WORD);
    ASSERT_EQ(subExpression3->token.value, "\\w");
    ASSERT_EQ(subExpression4->token.type, TokenType::ANY_SINGLE_DIGIT);
    ASSERT_EQ(subExpression4->token.value, "\\d");
}

TEST(ParserTest, rangeQuantifer)
{
    {
    Parser parser("a{2}");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::RANGE_QUANTIFER);
    ASSERT_EQ(ast->topNode->token.value, "2");
    ASTNode*expr = ast->topNode->leftChild;
    ASSERT_EQ(expr->token.type, TokenType::CHAR);
    ASSERT_EQ(expr->token.value, "a");
    }

    {
    Parser parser("a{2,}");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::RANGE_QUANTIFER);
    ASSERT_EQ(ast->topNode->token.value, "2,");
    ASTNode*expr = ast->topNode->leftChild;
    ASSERT_EQ(expr->token.type, TokenType::CHAR);
    ASSERT_EQ(expr->token.value, "a");
    }

    {
    Parser parser("a{2,3}");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_EQ(ast->topNode->token.type, TokenType::RANGE_QUANTIFER);
    ASSERT_EQ(ast->topNode->token.value, "2,3");
    ASTNode*expr = ast->topNode->leftChild;
    ASSERT_EQ(expr->token.type, TokenType::CHAR);
    ASSERT_EQ(expr->token.value, "a");
    }
    {
    Parser parser("a{2:3}");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_FALSE(ast->isVaild);
    }
}

TEST(ParserTest, SomeInvalidSyntax)
{
    {
    Parser parser("a{23");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_FALSE(ast->isVaild);
    ASSERT_EQ(ast->errorMsg, "Expect '}");
    }
    {
    Parser parser("[a-z");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_FALSE(ast->isVaild);
    ASSERT_EQ(ast->errorMsg, "Expect ']");
    }
    {
    Parser parser("(a-z");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_FALSE(ast->isVaild);
    ASSERT_EQ(ast->errorMsg, "Expect ')");
    }
}

TEST(ParserTest, gotTheCache)
{
    Parser parser("(\\w|\\d|a)*[0-9a-z]{3}+-/*");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_TRUE(ast->isVaild);
    std::shared_ptr<AST>ast1 = parser.parse();
    ASSERT_TRUE(ast1->isVaild);
    ASSERT_EQ(ast->topNode, ast1->topNode);
}

TEST(ParserTest, zeroOrOneGroupWhichHaszeroOrOneElementFllowAElement)
{
    //(xa?0)?
    Parser parser("(xa?0)?");
    std::shared_ptr<AST>ast = parser.parse();
    ASSERT_TRUE(ast->isVaild);
    ASTNode *topNode = ast->topNode;
    ASSERT_EQ(topNode->token.type, TokenType::RANGE_QUANTIFER);
    
    //(xa?0)
    ASTNode *xa0 = topNode->leftChild;
    ASSERT_EQ(xa0->token.type, TokenType::CAT);

    //xa?
    ASTNode *xa = xa0->leftChild;
    ASSERT_EQ(xa->token.type, TokenType::CAT);
    ASTNode *x = xa->leftChild;
    ASSERT_EQ(x->token.type, TokenType::CHAR);
    ASTNode *a = xa->rightChild;
    ASSERT_EQ(a->token.type, TokenType::RANGE_QUANTIFER);
    ASSERT_EQ(a->leftChild->token.type, TokenType::CHAR);
    ASSERT_EQ(a->leftChild->token.value, "a");

    //0
    ASSERT_EQ(xa0->rightChild->token.type, TokenType::CHAR);
    ASSERT_EQ(xa0->rightChild->token.value, "0");
}