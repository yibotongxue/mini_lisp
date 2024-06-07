#ifndef __REPL_H__
#define __REPL_H__

#include "../include/parser.h"
#include "../include/tokenizer.h"
#include "../include/eval_env.h"

class Repl {
private:
    int leftParen;
    int rightParen;
public:
    void readInput(std::shared_ptr<EvalEnv>& env);
};

#endif