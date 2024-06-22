#include "reader.h"

#include <iostream>
#include <stack>

#include "error.h"

/**
 * @brief 获取 Reader 类的单例实例。
 *
 * @return 静态的 Reader 对象
 */
Reader& Reader::getInstance() {
    static Reader reader;
    return reader;
}

/**
 * @brief 检查缓存是否为空。
 *
 * @return 如果缓存为空，返回 true；否则返回 false
 */
bool Reader::empty() const {
    return cache.empty();
}

/**
 * @brief 从标准输入读取输入并解析成 Token。
 *
 * @details 处理括号匹配、缩进和多行输入。
 *          将解析得到的 Token 放入缓存中。
 */
void Reader::readIn() {
    std::stack<int> leftParen{};
    std::deque<TokenPtr> tokens{};
    int indentationLevel = 1;  // 记录当前缩进层级

    while (true) {
        std::string line;
        std::getline(std::cin, line);

        for (int i = 0; i < static_cast<int>(line.size()); i++) {
            if (line[i] == '(') {
                leftParen.push(indentationLevel + i);
            } else if (line[i] == ')') {
                if (leftParen.empty()) {
                    throw SyntaxError("Unmatched parens.");
                }
                leftParen.pop();
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

        if (tokens.back()->getType() != TokenType::QUOTE &&
            tokens.back()->getType() != TokenType::QUASIQUOTE &&
            tokens.back()->getType() != TokenType::UNQUOTE &&
            leftParen.empty()) {
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

/**
 * @brief 获取缓存中的下一个值。
 *
 * @return 缓存中的值
 */
ValuePtr Reader::output() {
    auto value = cache.front();
    cache.pop_front();
    return value;
}