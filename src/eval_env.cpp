/* ************************************************************************
> File Name:     eval_env.cpp
> Author:        林毅波
> Created Time:  2024/5/17 15:49:20
> Description:   
 ************************************************************************/
#include "../include/eval_env.h"
#include "../include/error.h"
#include <iostream>

using namespace std::literals;

extern void backtracking(std::vector<ValuePtr>& v, ValuePtr ptr);

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating()) {
        return expr;
    }
    else if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    }
    else if (expr->isList()) {
        auto ptr = std::dynamic_pointer_cast<PairValue>(expr);
        std::vector<ValuePtr> v{};
        backtracking(v, expr);
        if (v[0]->asSymbol() == "define"s) {
            if (auto name = v[1]->asSymbol()) {
                symbolList[std::dynamic_pointer_cast<SymbolValue>(v[1])->getName()] = eval(v[2]);
                return std::make_shared<NilValue>();
            }
            else {
                throw LispError("Malformed define.");
            }
        }
        else {
            throw LispError("Unimplemented");
        }
    }
    else if (expr->isSymbol()) {
        auto name = std::dynamic_pointer_cast<SymbolValue>(expr)->getName();
        auto it = symbolList.find(name);
        if(it == symbolList.end())
            throw LispError("Variable " + name + " not defined.");
        else
            return ValuePtr(it->second);
    }
    else {
        throw LispError("Unimplemented");
    }
}
