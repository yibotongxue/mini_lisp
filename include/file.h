#ifndef __FILE_H__
#define __FILE_H__

#include "./tokenizer.h"
#include "./parser.h"
#include "./eval_env.h"

class File {
private:
    std::string fileName;

    File(const std::string& fileName) : fileName{fileName} {}
    void carryOut(std::shared_ptr<EvalEnv>& env);

    std::deque<TokenPtr> readTokens(); 

public:
    static void carryOut(const std::string& fileName, std::shared_ptr<EvalEnv>& env);
};

#endif