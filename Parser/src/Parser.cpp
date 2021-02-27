#include <memory>
#include <Parser/AST.hpp>
#include <Parser/Parser.hpp>
#include <Scanner/Scanner.hpp>

namespace{
    bool isQuantifierType(TokenType type)
    {
        return TokenType::ZERO_MORE == type ||
                TokenType::ZERO_OR_ONE == type ||
                TokenType::ONE_MORE == type ||
                TokenType::LEFT_BRACE == type;
    }

    ASTNode*copyNodes(ASTNode*node)
    {
       ASTNode*topNode = new ASTNode(node->token);
        if(node->leftChild != nullptr){
            topNode->leftChild = copyNodes(node->leftChild);
        }
        if(node->rightChild != nullptr){
            topNode->rightChild = copyNodes(node->rightChild);
        }
        return topNode;
    }

    void cleanNodes(ASTNode *node)
    {
        AST ast;
        ast.topNode = node;
    }

};

struct Parser::Impl{

    //properties
    std::unique_ptr<Scanner> scanner;

    Token currentToken;

    std::shared_ptr<AST> astCache;

    //methods
    ASTNode*oneMore(ASTNode*src)
    {
        Token catToken = {TokenType::CAT, ""};
        ASTNode*cat = new ASTNode(catToken);
        cat->leftChild = src;
        //*
        Token startoken = {TokenType::ZERO_MORE, "*"};
        cat->rightChild = new ASTNode(startoken);
        ASTNode*srcCopy = copyNodes(src);
        cat->rightChild->leftChild = srcCopy;
        consume(TokenType::ONE_MORE);
        return cat;
    }

    bool consume(TokenType type)
    {
        if(currentToken.type != type){
            return false;
        }
        currentToken = scanner->getNextToken();
        return true;
    }

    ASTNode*CreateNodeForCurrentToken()
    {
        ASTNode*node = new ASTNode(currentToken);
        currentToken = scanner->getNextToken();
        return node;
    }

    ASTNode*characterClass()
    {
        return CreateNodeForCurrentToken();
    }

    ASTNode*matchCharacter()
    {
        ASTNode*character = CreateNodeForCurrentToken();
        character->token.type = TokenType::CHAR;
        return character;
    }

    ASTNode*CharacterRange()
    {
        ASTNode*range = nullptr;
        ASTNode*start = matchCharacter();
        range = start;
        if(TokenType::CHARACTER_RANGE == currentToken.type){
            range = CreateNodeForCurrentToken();
            ASTNode*end = matchCharacter();
            range->leftChild = start;
            range->rightChild = end;
        }
        return range;
    }

    ASTNode*characterGroupItem()
    {
        switch (currentToken.type)
        {
            case TokenType::ANY_SINGLE_NOT_DIGIT:
            case TokenType::ANY_SINGLE_DIGIT:
            case TokenType::ANY_SINGLE_NOT_WORD:
            case TokenType::ANY_SINGLE_WORD:
                return characterClass();
        
            case  TokenType::CHAR:
                return CharacterRange();
            default:
                break;
        }
        return nullptr;
    }

    ASTNode*characterGroup()
    {
        ASTNode*group = nullptr;
        if(!consume(TokenType::LEFT_BRACKET)){
            return nullptr;
        }
        if(TokenType::STRING_START_ANCHOR == currentToken.type){
            currentToken = scanner->getNextToken();
            //TODO
        }
        while (TokenType::RIGHT_BRACKET != currentToken.type &&
                TokenType::END != currentToken.type)
        {
            ASTNode*groupItem = characterGroupItem();
            if(nullptr == group){
                group = groupItem;
            }else{
                Token token = {TokenType::OR, "|"};
                ASTNode*cat = new ASTNode(token);
                cat->leftChild = group;
                cat->rightChild = groupItem;

                group = cat;
            }
        }
        
        if(!consume(TokenType::RIGHT_BRACKET)){
            astCache->errorMsg = "Expect ']";
            cleanNodes(group);
            return nullptr;
        }
        return group;
    }

    ASTNode*matchCharacterClass()
    {
        if(TokenType::LEFT_BRACKET == currentToken.type){
            return  characterGroup();
        }
        return characterClass();
    }

    ASTNode*matchItem()
    {
        if(TokenType::LEFT_BRACKET == currentToken.type ||
            TokenType::ANY_SINGLE_NOT_DIGIT == currentToken.type ||
            TokenType::ANY_SINGLE_DIGIT == currentToken.type ||
            TokenType::ANY_SINGLE_NOT_WORD == currentToken.type ||
            TokenType::ANY_SINGLE_WORD == currentToken.type){
            return matchCharacterClass();
        } else if(TokenType::ANY_SIGLE_CHAR_EXCEPT_NEWLINE == currentToken.type){
            return CreateNodeForCurrentToken();
        } else{
            return matchCharacter();
        }
    }

    ASTNode*rangeQuantifer()
    {
        if(!consume(TokenType::LEFT_BRACE)){
            return nullptr;
        }
        Token token = {TokenType::RANGE_QUANTIFER, ""};
        ASTNode*node = new ASTNode(token);
        while(TokenType::CHAR == currentToken.type &&
            isdigit(currentToken.value[0]))
        {
            node->token.value += currentToken.value;
            consume(TokenType::CHAR);
        }
        if (TokenType::COMMA == currentToken.type &&
            ',' == currentToken.value[0])
        {
            node->token.value += ',';
            consume(TokenType::COMMA);
            while(TokenType::CHAR == currentToken.type &&
                isdigit(currentToken.value[0]))
            {
                node->token.value += currentToken.value;
                consume(TokenType::CHAR);
            }
        }
        if(!consume(TokenType::RIGHT_BRACE)){
            astCache->errorMsg = "Expect '}";
            cleanNodes(node);
            return nullptr;
        }
        return node;
    }

    ASTNode*quantifier()
    {
        if(TokenType::LEFT_BRACE == currentToken.type){
            return rangeQuantifer();
        }
        return CreateNodeForCurrentToken();
    }

    ASTNode*match()
    {
        ASTNode*item = matchItem();
        if(isQuantifierType(currentToken.type)){
            if(TokenType::ONE_MORE == currentToken.type){
                return oneMore(item);
            }else{
                ASTNode*quan = quantifier();
                if(nullptr == quan){
                    cleanNodes(item);
                    return nullptr;
                }
                quan->leftChild = item;
                return quan;
            }
        }
        return item;
    }

    ASTNode*group()
    {
        ASTNode*group = nullptr;
        if(!consume(TokenType::LEFT_PAREN)){
            return nullptr;
        }
        ASTNode*expr = expression();
        if(!consume(TokenType::RIGHT_PAREN)){
            astCache->errorMsg = "Expect ')";
            cleanNodes(expr);
            return nullptr;
        }
        if(isQuantifierType(currentToken.type)){
            if (TokenType::ONE_MORE == currentToken.type)
            {
                return oneMore(expr);
            }
            else
            {
                group = CreateNodeForCurrentToken();
                group->leftChild = expr;
            }
        } else {
            group = expr;
        }
        return group;
    }

    ASTNode*subExpressionItem()
    {
        if(TokenType::LEFT_PAREN == currentToken.type){
            return group();
        }
        return match();
    }

    ASTNode*subExpression()
    {
        ASTNode*subExpression = nullptr;
        while (TokenType::END != currentToken.type &&
                TokenType::OR != currentToken.type &&
                TokenType::RIGHT_PAREN != currentToken.type
                ){
            ASTNode*item = subExpressionItem();
            if(item == nullptr){
                cleanNodes(subExpression);
                return nullptr;
            }
            if(subExpression != nullptr){
                ASTNode*cat = new ASTNode({TokenType::CAT, ""});
                cat->leftChild = subExpression;
                cat->rightChild = item;

                subExpression = cat;
            }else{
                subExpression = item;
            }
        } 
        return subExpression;
    }

    ASTNode*expression()
    {
        ASTNode *expression = nullptr;
        if (TokenType::STRING_START_ANCHOR == currentToken.type)
        {
            //TODO
            currentToken = scanner->getNextToken();
        }
        do
        {
            if (expression != nullptr)
            {
                ASTNode *un = CreateNodeForCurrentToken();
                un->leftChild = expression;
                un->rightChild = subExpression();
                expression = un;
            }
            else
            {
                expression = subExpression();
            }
        } while (TokenType::OR == currentToken.type);
        return expression;
    }
};

Parser::Parser(const std::string &input)
    :impl_(new Impl)
{
    impl_->scanner = std::make_unique<Scanner>(input);
}

Parser::~Parser() = default;

std::shared_ptr<AST> Parser::parse()
{
    if(impl_->astCache != nullptr){
        return impl_->astCache;
    }
    impl_->astCache = std::make_shared<AST>();
    impl_->currentToken = impl_->scanner->getNextToken();
    if (TokenType::END == impl_->currentToken.type)
    {
        return nullptr;
    }
    ASTNode*expression = impl_->expression();
    if(expression != nullptr){
        impl_->astCache->isVaild = true;
    }
    impl_->astCache->topNode = expression;
    return impl_->astCache;
}