#include "../include/repl.h"
#include "../include/error.h"
#include <iostream>
#include <string>

/**
 * @brief 从标准输入读取Token并构建Token队列。
 * 
 * @return Token队列
 * @throws SyntaxError 如果括号不匹配
 */
std::deque<TokenPtr> Repl::readTokens() {
    int leftParen{0}, rightParen{0};
    std::deque<TokenPtr> tokens{};
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        if (std::cin.eof()) {
            std::exit(0);
        }
        auto lineTokens = Tokenizer::tokenize(line);
        for (auto& token : lineTokens) {
            if (token->getType() == TokenType::LEFT_PAREN) {
                leftParen++;
            }
            else if (token->getType() == TokenType::RIGHT_PAREN) {
                rightParen++;
            }
            if (leftParen < rightParen) {
                throw SyntaxError("Unmatched parens.");
            }
            tokens.push_back(std::move(token));
        }
        if (tokens.back()->getType() != TokenType::QUOTE 
            && tokens.back()->getType() != TokenType::QUASIQUOTE 
            && tokens.back()->getType() != TokenType::UNQUOTE 
            && leftParen == rightParen) {
                return std::move(tokens);
        }
        std::cout << "... "; 
    }
}

/**
 * @brief 执行REPL中的表达式。
 * 
 * @param env 求值环境
 */
void Repl::carryOut(std::shared_ptr<EvalEnv>& env) {
    auto tokens = Repl().readTokens();
    Parser parser(std::move(tokens));
    while(!parser.empty()) {
        auto value = parser.parse();
        auto result = env->eval(value);
        std::cout << result->toString() << std::endl;
    }
}