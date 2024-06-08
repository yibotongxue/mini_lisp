#ifndef __REPL_H__
#define __REPL_H__

#include "../include/parser.h"
#include "../include/tokenizer.h"
#include "../include/eval_env.h"

class Repl {
private:
    Repl() = default;
    std::deque<TokenPtr> readTokens();
public:
    static void carryOut(std::shared_ptr<EvalEnv>& env);
};

#endif