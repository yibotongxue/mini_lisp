#include "reader.h"
#include "error.h"
#include <iostream>
#include <stack>

Reader& Reader::getInstance() {
    static Reader reader;
    return reader;
}

bool Reader::empty() const {
    return cache.empty();
}

void Reader::readIn() {
    std::stack<int> leftParen{};
    std::deque<TokenPtr> tokens{};
    int indentationLevel = 1; // 记录当前缩进层级

    while (true) {
        std::string line;
        std::getline(std::cin, line);

        for (int i = 0; i < static_cast<int>(line.size()); i++) {
            if (line[i] == '(') {
                leftParen.push(indentationLevel + i);
            } else if (line[i] == ')') {
                if (!leftParen.empty()) {
                    leftParen.pop();
                } else {
                    throw SyntaxError("Unmatched parens.");
                }
            }
        }

        if (std::cin.eof()) {
            std::exit(0);
        }

        auto lineTokens = Tokenizer::tokenize(line);
        if (lineTokens.size() == 0) {
            if (leftParen.empty()) {
                std::cout << "... ";
                continue;
            }
            std::cout << "... " << std::string(leftParen.top(), ' ');
            indentationLevel = leftParen.top() + 1;
            continue;
        }

        for (auto& token : lineTokens) {
            tokens.push_back(std::move(token));
        }

        if (tokens.back()->getType() != TokenType::QUOTE 
            && tokens.back()->getType() != TokenType::QUASIQUOTE 
            && tokens.back()->getType() != TokenType::UNQUOTE 
            && leftParen.empty()) {
                Parser parser(std::move(tokens));
                while (!parser.empty()) {
                    cache.push_back(parser.parse());
                }
                return;
        }
        indentationLevel = leftParen.top() + 1;
        std::cout << "... " << std::string(leftParen.top(), ' '); 
    }
}

ValuePtr Reader::output() {
    auto value = cache.front();
    cache.pop_front();
    return value;
}