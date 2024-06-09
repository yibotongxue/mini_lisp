#ifndef __MODEL_H__
#define __MODEL_H__

#include "parser.h"
#include "tokenizer.h"
#include "eval_env.h"

class Model {
protected:
    virtual ~Model() = default;
    virtual std::deque<TokenPtr> readTokens() = 0;
public:
    virtual void carryOut(std::shared_ptr<EvalEnv>& env) = 0;
};

class Repl : public Model {
private:
    std::deque<TokenPtr> readTokens() override;
public:
    void carryOut(std::shared_ptr<EvalEnv>& env) override;   
};

class File : public Model {
private:
    std::string fileName;
    std::deque<TokenPtr> readTokens() override;
public:
    File(const std::string& fileName) : fileName{fileName} {}
    void carryOut(std::shared_ptr<EvalEnv>& env) override;  
};

#endif