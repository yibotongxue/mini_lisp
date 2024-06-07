/* ************************************************************************
> File Name:     parser.cpp
> Author:        林毅波
> Created Time:  2024/5/12 13:09:25
> Description:   类Parser的实现
 ************************************************************************/

#include "../include/parser.h"
#include "../include/error.h"
#include <iostream>

Parser::Parser() : tokens { {} } {}

Parser::Parser(std::deque<TokenPtr> tokens) : tokens { std::move(tokens) } {}

/**
 * @brief 解析函数，解析 tokens 中的词法标记并生成相应的值对象
 * 
 * 解析函数从 tokens 中读取词法标记，根据标记类型生成相应的值对象(Value)。
 * 如果 tokens 为空，则抛出 SyntaxError 异常。
 * 
 * @return 返回解析出的值对象的共享指针
 * 
 * @throws SyntaxError 当 tokens 为空或者遇到未实现的词法标记时抛出异常
 */
ValuePtr Parser::parse() {
    if(tokens.empty()) {
        throw SyntaxError("Empty tokens"); // 如果 tokens 为空，则抛出异常
    }

    auto token = std::move(tokens.front()); // 从 tokens 中获取第一个词法标记
    tokens.pop_front(); // 移除已处理的词法标记

    if(token->getType() == TokenType::NUMERIC_LITERAL) { // 如果词法标记是数值字面量
        auto value = static_cast<NumericLiteralToken&>(*token).getValue(); // 获取数值字面量的值
        return std::make_shared<NumericValue>(value); // 返回数值类型值对象的共享指针
    }
    else if(token->getType() == TokenType::BOOLEAN_LITERAL) { // 如果词法标记是布尔字面量
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue(); // 获取布尔字面量的值
        return std::make_shared<BooleanValue>(value); // 返回布尔类型对象的共享指针
    }
    else if(token->getType() == TokenType::STRING_LITERAL) { // 如果词法标记是字符串字面能量
        auto value = static_cast<StringLiteralToken&>(*token).getValue(); // 获取字符串字面量的值
        return std::make_shared<StringValue>(value); // 返回字符串类型值对象的共享指针
    }
    else if(token->getType() == TokenType::IDENTIFIER) { // 如果词法标记是标识符
        auto value = static_cast<IdentifierToken&>(*token).getName(); // 获取标识符的名称
        return std::make_shared<SymbolValue>(value); // 返回符号类型值对象的共享指针
    }
    else if(token->getType() == TokenType::LEFT_PAREN) { // 如果词法标记是左括号
        return this->parseTails(); // 递归调用 S-表达式的函数并返回结果
    }
    else if(token->getType() == TokenType::QUOTE) { // 如果词法标记是引用
        // 构造一个以 'quote' 为左元素，当前解析的值为右元素的对子，并返回其共享指针
        return std::make_shared<PairValue>(
          std::make_shared<SymbolValue>("quote"),
          std::make_shared<PairValue>(
              this->parse(), // 递归调用解析函数获取当前位置的值
              std::make_shared<NilValue>() // 创建一个空表作为右元素
              )
          );
    }
    else if(token->getType() == TokenType::QUASIQUOTE) { // 如果词法标记是quasiquote
        // 构造一个以'quasiquote' 为做元素，当前解析的值为右元素的对子，并返回其共享指针
        return std::make_shared<PairValue>(
          std::make_shared<SymbolValue>("quasiquote"),
          std::make_shared<PairValue>(
              this->parse(), // 递归调用解析函数获取当前位置的值
              std::make_shared<NilValue>() // 创建一个空表作为右元素
              )
          );
    }
    else if(token->getType() == TokenType::UNQUOTE) { // 如果词法标记是unquote
        // 构造一个以 'unquote' 为做元素，当前解析的值为右元素的对子，并返回其共享指针
        return std::make_shared<PairValue>(
          std::make_shared<SymbolValue>("unquote"),
          std::make_shared<PairValue>(
              this->parse(), // 递归调用解析函数获取当前位置的值
              std::make_shared<NilValue>() // 创建一个空表作为右元素
              )
          );
    }
    else {
        throw SyntaxError("Unimplemented in parser.cpp, line 81"); // 如果遇到为实现的词法标记，则抛出异常
    }
}

/**
 * @brief 解析 S-表达式
 * 
 * 解析 S-表达式，即对子的表示形式，如 (a b c)，(1 2 3)，等。
 * 如果 tokens 的第一个词法标记是右括号，则返回空表(NilValue)。
 * 否则，递归地调用 parse 函数解析左右部分，并构造对子(PairValue)。
 * 
 * @return 返回解析出的对子或空表的值对象的共享指针
 * 
 * @throws SyntaxError 当遇到无法匹配的括号或者 tokens 已经遍历完时抛出异常
 */
ValuePtr Parser::parseTails() {
    if (tokens.empty()) {
        throw SyntaxError("Unmatched parens");
    }
    try{
        if(tokens.front()->getType() == TokenType::RIGHT_PAREN) { // 如果下一个词法标记是右括号
            tokens.pop_front(); // 移除右括号
            return std::make_shared<NilValue>(); // 返回 空表类型值对象的共享指针
        }
    }
    catch(...) {
        throw SyntaxError("eof"); // 入股 tokens 一场为空，则抛出 eof 异常
    }
    auto car = this->parse(); // 解析当前位置的值作为对子的左侧
    if (tokens.empty()) {
        throw SyntaxError("Unmatched parens");
    }
    if(tokens.front()->getType() == TokenType::DOT) { // 如果下一个词法标记是点号
        tokens.pop_front(); // 移除点号
        auto cdr = this->parse(); // 解析点号后的值作为对子的右侧
        if(tokens.empty() || tokens.front()->getType() != TokenType::RIGHT_PAREN) { // 如果点号后的词法标记不是右括号
            throw SyntaxError("Unmatched parens"); // 抛出无法匹配的括号异常
        }
        tokens.pop_front(); // 移除右括号
        return std::make_shared<PairValue>(car, cdr); // 返回构造的对子类型值对象的共享指针
    } else {
        auto cdr = this->parseTails(); //递归调用解析 S-表达式的函数获取右侧值
        return std::make_shared<PairValue>(car, cdr); // 返回构造的肚子类型值对象的共享指针
    }
}

bool Parser::empty() const {
    return tokens.empty();
}

void Parser::check() const {
    if (!tokens.empty()) {
        for (auto& token : tokens) {
            std::cout << token->toString() << std::endl;
        }
        throw SyntaxError("Unimplement in parser.cpp, line 129.");
    }
}
