#include "../include/file.h"
#include "../include/error.h"
#include <fstream>
#include <algorithm>
#include <iterator>

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

void File::carryOut(std::shared_ptr<EvalEnv>& env) {
    auto tokens = readTokens();
    Parser parser(std::move(tokens));
    while(!parser.empty()) {
        auto value = parser.parse();
        env->eval(std::move(value));
    }

    // auto lines = readLines();
    // for (auto& line : lines) {
    //     carry(line, env);
    // }
}

void File::carryOut(const std::string& fileName, std::shared_ptr<EvalEnv>& env) {
    File(fileName).carryOut(env);
}