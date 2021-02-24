#include <memory>
#include <CommonDataStructure/AST.hpp>
#include <Parser/Parser.hpp>
#include <Scanner/Scanner.hpp>

namespace{
    Node *createNode(Token &token)
    {
        Node *node = new Node;
        node->token = token;
        node->leftChild = node->rightChild = nullptr;
        return node;
    }

    bool isQuantifierType(TokenType type)
    {
        return TokenType::ZERO_MORE == type ||
                TokenType::ZERO_OR_ONE == type ||
                TokenType::ONE_MORE == type;
    }

    Node *copyNodes(Node *node)
    {
       Node *topNode = new Node;
       topNode->token = node->token;
        if(node->leftChild != nullptr){
            topNode->leftChild = copyNodes(node->leftChild);
        }
        if(node->rightChild != nullptr){
            topNode->leftChild = copyNodes(node->rightChild);
        }
        return topNode;
    }
};

struct Parser::Impl{
    std::unique_ptr<Scanner> scanner;

    Token currentToken;

    bool consume(TokenType type)
    {
        if(currentToken.type != type){
            return false;
        }
        currentToken = scanner->getNextToken();
        return true;
    }

    Node *CreateNodeForCurrentToken()
    {
        Node *node = createNode(currentToken);
        currentToken = scanner->getNextToken();
        return node;
    }

    Node *characterClass()
    {
        return CreateNodeForCurrentToken();
    }

    Node *matchCharacter()
    {
        Node *character = CreateNodeForCurrentToken();
        character->token.type = TokenType::CHAR;
        return character;
    }

    Node *CharacterRange()
    {
        Node *range = nullptr;
        Node *start = matchCharacter();
        range = start;
        if(TokenType::CHARACTER_RANGE == currentToken.type){
            range = CreateNodeForCurrentToken();
            Node *end = matchCharacter();
            range->leftChild = start;
            range->rightChild = end;
        }
        return range;
    }

    Node *characterGroupItem()
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

    Node *characterGroup()
    {
        Node *group = nullptr;
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
            Node *groupItem = characterGroupItem();
            if(nullptr == group){
                group = groupItem;
            }else{
                Node *cat = new Node;
                cat->token = {TokenType::CAT, ""};
                cat->leftChild = group;
                cat->rightChild = groupItem;

                group = cat;
            }
        }
        
        if(!consume(TokenType::RIGHT_BRACKET)){
                abort();
            }
        return group;
    }

    Node *matchCharacterClass()
    {
        if(TokenType::LEFT_BRACKET == currentToken.type){
            return  characterGroup();
        }
        return characterClass();
    }

    Node *matchItem()
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

    Node *quantifier()
    {
        return CreateNodeForCurrentToken();
    }

    Node *match()
    {
        Node *item = matchItem();
        if(TokenType::ONE_MORE == currentToken.type){
            Node *cat = new Node;
            cat->token = {TokenType::CAT, ""};
            cat->leftChild = item;
            //*
            Token token;
            token.type = TokenType::ZERO_MORE;
            token.value = "*";
            cat->rightChild = createNode(token);
            Node *itemCopy = copyNodes(item);
            cat->rightChild->leftChild = itemCopy;
            consume(TokenType::ONE_MORE);
            return cat;
        }
        if(TokenType::ZERO_MORE == currentToken.type ||
            TokenType::ZERO_OR_ONE == currentToken.type){
            Node *quan = quantifier();
            quan->leftChild = item;
            return quan;
        }
        return item;
    }

    Node *group()
    {
        Node *group = nullptr;
        if(!consume(TokenType::LEFT_PAREN)){
            return nullptr;
        }
        Node *expr = expression();
        if(!consume(TokenType::RIGHT_PAREN)){
            return nullptr;
        }
        if(isQuantifierType(currentToken.type)){
            group = CreateNodeForCurrentToken();
            group->leftChild = expr;
        } else {
            group = expr;
        }
        return group;
    }

    Node *subExpressionItem()
    {
        if(TokenType::LEFT_PAREN == currentToken.type){
            return group();
        }
        return match();
    }

    Node *subExpression()
    {
        Node *subExpression = nullptr;
        while (TokenType::END != currentToken.type &&
                TokenType::OR != currentToken.type &&
                TokenType::RIGHT_PAREN != currentToken.type
                ){
            Node *item = subExpressionItem();
            if(subExpression != nullptr){
                Node *cat = new Node;
                cat->token = {TokenType::CAT, ""};
                cat->leftChild = subExpression;
                cat->rightChild = item;

                subExpression = cat;
            }else{
                subExpression = item;
            }
        } 
        return subExpression;
    }

    Node *expression()
    {
        Node *expression = nullptr;
        if(TokenType::STRING_START_ANCHOR == currentToken.type){
            //TODO
            currentToken = scanner->getNextToken();
        }
        do{
            if(expression != nullptr){
                Node *un = CreateNodeForCurrentToken();
                un->leftChild = expression;
                un->rightChild = subExpression();
                expression = un;
            } else{
                expression = subExpression();
            }
        }while(TokenType::OR == currentToken.type);
        return expression;
    }
};

Parser::Parser(const std::string &input)
    :impl_(new Impl)
{
    impl_->scanner = std::make_unique<Scanner>(input);
}

Parser::~Parser() = default;

AST *Parser::parse()
{
    impl_->currentToken = impl_->scanner->getNextToken();
    if(TokenType::END == impl_->currentToken.type){
        return nullptr;
    }
    AST *tree = new AST;
    Node *expression = impl_->expression();
    tree->topNode = expression;
    return tree;
}