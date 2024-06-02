#include "../include/file.h"
#include "../include/error.h"
#include <fstream>

std::vector<std::string> File::readLines(const std::string& fileName) {
    std::vector<std::string> lines{};
    std::ifstream file{ fileName };

    if (!file.is_open()) {
        throw FileError("Open the file failed.");
    }

    if (file.fail()) {
        throw FileError("File failed.");
    }

    std::string line;
    
    while(std::getline(file, line)) {
        if (!line.empty())
            lines.push_back(line);
    }

    if (lines.empty()) {
        throw FileError("The file is empyt.");
    }

    return lines;
}

void File::carry(const std::string& line, std::shared_ptr<EvalEnv>& env) {
    auto tokens = Tokenizer::tokenize(line);
    Parser parser(std::move(tokens));
    auto value = parser.parse();
    env->eval(std::move(value));
}

void File::carryOut(std::shared_ptr<EvalEnv>& env) {
    auto lines = readLines(fileName);
    for (auto& line : lines) {
        carry(line, env);
    }
}

void File::carryOut(const std::string& fileName, std::shared_ptr<EvalEnv>& env) {
    File(fileName).carryOut(env);
}