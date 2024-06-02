#ifndef __FILE_H__
#define __FILE_H__

#include "./tokenizer.h"
#include "./parser.h"
#include "./eval_env.h"

class File {
private:
    std::string fileName;

    File(const std::string& fileName) : fileName{fileName} {}
    std::vector<std::string> readLines(const std::string& fileName);
    void carry(const std::string& line, std::shared_ptr<EvalEnv>& env);
    void carryOut(std::shared_ptr<EvalEnv>& env);

public:
    static void carryOut(const std::string& fileName, std::shared_ptr<EvalEnv>& env);
};

#endif