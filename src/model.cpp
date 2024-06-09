#include "../include/model.h"
#include "../include/error.h"
#include <iostream>
#include <stack>
#include <string>
#include <fstream>

/**
 * @brief 从标准输入读取Token并构建Token队列。
 * 
 * @return Token队列
 * @throws SyntaxError 如果括号不匹配
 */
std::deque<TokenPtr> Repl::readTokens() {
    std::stack<int> leftParen{};
    std::deque<TokenPtr> tokens{};
    int indentationLevel = 1; // 记录当前缩进层级

    while (true) {
        std::string line;
        std::getline(std::cin, line);

        // 更新缩进层级
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
        indentationLevel = leftParen.size();

        if (std::cin.eof()) {
            std::exit(0);
        }

        auto lineTokens = Tokenizer::tokenize(line);
        if (lineTokens.size() == 0) {
            if (tokens.empty()) {
                return std::move(tokens);
            } else {
                std::cout << "... " << std::string(leftParen.top(), ' ');
                indentationLevel = leftParen.top() + 1;
                continue;
            }
        }

        for (auto& token : lineTokens) {
            tokens.push_back(std::move(token));
        }

        if (tokens.back()->getType() != TokenType::QUOTE 
            && tokens.back()->getType() != TokenType::QUASIQUOTE 
            && tokens.back()->getType() != TokenType::UNQUOTE 
            && leftParen.empty()) {
                return std::move(tokens);
        }
        indentationLevel = leftParen.top() + 1;
        std::cout << "... " << std::string(leftParen.top(), ' '); 
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

/**
 * @brief 从文件中读取Token。
 * 
 * @return 存储Token的deque
 * @throws FileError 如果打开或读取文件失败
 */
std::deque<TokenPtr> File::readTokens() {
    std::deque<TokenPtr> tokens{};
    std::ifstream file{ fileName };

    if (!file.is_open()) {
        throw FileError("Open the file failed.");
    }

    if (file.fail()) {
        throw FileError("File failed.");
    }

    std::string line;
    
    while(std::getline(file, line)) {
        auto lineTokens = std::move(Tokenizer::tokenize(line));
        for (auto& token : lineTokens) {
            tokens.push_back(std::move(token));
        }
    }

    return tokens;
}

/**
 * @brief 执行文件中的表达式。
 * 
 * @param env 求值环境
 */
void File::carryOut(std::shared_ptr<EvalEnv>& env) {
    auto tokens = readTokens();
    Parser parser(std::move(tokens));
    while(!parser.empty()) {
        auto value = parser.parse();
        env->eval(std::move(value));
    }
}