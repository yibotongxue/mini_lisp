/* ************************************************************************
> File Name:     eval_env.cpp
> Author:        林毅波
> Created Time:  2024/5/17 15:49:20
> Description:   
 ************************************************************************/
#include "../include/eval_env.h"
#include "../include/error.h"

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating()) {
        return expr;
    }
    else if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    }
    else {
        throw LispError("Unimplemented");
    }
}
