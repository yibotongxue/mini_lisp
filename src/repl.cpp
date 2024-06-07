#include "../include/repl.h"
#include <iostream>
#include <string>

void Repl::readInput(std::shared_ptr<EvalEnv>& env) {
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
            tokens.push_back(std::move(token));
        }
        if (tokens.back()->getType() != TokenType::QUOTE 
            && tokens.back()->getType() != TokenType::QUASIQUOTE 
            && tokens.back()->getType() != TokenType::UNQUOTE 
            && leftParen == rightParen) {
                Parser parser(std::move(tokens));
                while (!parser.empty()) {
                    auto value = parser.parse();
                    auto result = env->eval(value);
                    std::cout << result->toString() << std::endl;
                }
                return;
        }
        std::cout << "... "; 
    }
}