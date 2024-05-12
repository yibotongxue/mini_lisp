/* ************************************************************************
> File Name:     parser.cpp
> Author:        林毅波
> Created Time:  2024/5/12 13:09:25
> Description:   类Parser的实现
 ************************************************************************/

#include "../include/parser.h"
#include "../include/error.h"

Parser::Parser(std::deque<TokenPtr> tokens) : tokens { tokens } {}

ValuePtr Parser::parse() {
    if(tokens.empty()) {
        throw SyntaxError("Empty tokens");
    }
    auto& token = tokens.front();
    if(token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        return std::make_shared<NumericValue>(value);
    }
    else if(token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        return std::make_shared<BooleanValue>(value);
    }
    else if(token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        return std::make_shared<StringValue>(value);
    }
    else if(token->getType() == TokenType::IDENTIFIER) {
        auto value = static_cast<IdentifierToken&>(*token).getName();
        return std::make_shared<SymbolValue>(value);
    }
    else {
        throw SyntaxError("Unimplemented");
    }
}
