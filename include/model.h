#ifndef __MODEL_H__
#define __MODEL_H__

#include "../include/parser.h"
#include "../include/tokenizer.h"
#include "../include/eval_env.h"

class Model {
protected:
    ~Model() = default;
public:
    virtual std::deque<TokenPtr> readTokens() = 0;
    virtual void carryOut(std::shared_ptr<EvalEnv>& env) = 0;
};

class Repl : public Model {
public:
    std::deque<TokenPtr> readTokens() override;
    void carryOut(std::shared_ptr<EvalEnv>& env) override;   
};

class File : public Model {
private:
    std::string fileName;
public:
    File(const std::string& fileName) : fileName{fileName} {}
    std::deque<TokenPtr> readTokens() override;
    void carryOut(std::shared_ptr<EvalEnv>& env) override;  
};

#endif