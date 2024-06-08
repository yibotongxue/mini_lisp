#include "../include/file.h"
#include "../include/error.h"
#include <fstream>
#include <algorithm>
#include <iterator>

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

/**
 * @brief 执行指定文件中的表达式。
 * 
 * @param fileName 文件名
 * @param env 求值环境
 */
void File::carryOut(const std::string& fileName, std::shared_ptr<EvalEnv>& env) {
    File(fileName).carryOut(env);
}