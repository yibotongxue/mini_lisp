/* ************************************************************************
> File Name:     builtins.cpp
> Author:        林毅波
> Created Time:  2024/5/23 21:26:41
> Description:   
 ************************************************************************/

#include "..\include\builtins.h"
#include "../include/error.h"
#include "../include/eval_env.h"
#include <iostream>

ValuePtr add(const std::vector<ValuePtr>& params, EvalEnv&) {
    double result = 0;
    for (const auto& i :params) {
        if (i->isNumber()) {
            result += *i->asNumber();
        }
        else {
            throw LispError("Cannot add a non-numeric value.");
        }
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv&) {
    for (const auto& param : params) {
        std::cout << param->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

ValuePtr multiply(const std::vector<ValuePtr>& params, EvalEnv&) {
    double result = 1;
    for (const auto& i : params) {
        if (i->isNumber()) {
            result *= *i->asNumber();
        }
        else {
            throw LispError("Cannot multiply a non-numeric value.");
        }
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr larger(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() < 2) {
        throw LispError("Less params than needed.");
    }
    else {
        bool result = true;
        if (!params[0]->isNumber()) {
            throw LispError("Cannot compare a non-numeric value.");
        }
        for (int i = 1; i < params.size(); i++) {
            if (params[i]->isNumber()) {
                result = result && *params[i]->asNumber() < *params[i - 1]->asNumber();
            }
            else {
                throw LispError("Cannot compare a non-numeric value.");
            }
        }
        return std::make_shared<BooleanValue>(result);
    }
}

ValuePtr reduce(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("Less params than needed.");
    }
    else if (params.size() == 1) {
        if (params[0]->isNumber()) {
            return std::make_shared<NumericValue>(-*params[0]->asNumber());
        }
        else {
            throw LispError("Cannot reduce a non-numeric value.");
        }
    }
    else {
        if (params[0]->isNumber()) {
            double result = *params[0]->asNumber();
            for (int i = 1; i < params.size(); i++) {
                if (params[i]->isNumber()) {
                    result -= *params[i]->asNumber();
                }
                else {
                    throw LispError("Cannot reduce a non-numeric value.");
                }
            }
            return std::make_shared<NumericValue>(result);
        }
        else {
            throw LispError("Cannot reduce a non-numeric value.");
        }
    }
}

ValuePtr apply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 1) {
        throw LispError("A param is needed.");
    }
    if (params.size() > 2) {
        throw LispError("The params are more than needed.");
    }
    if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
        if (params[1]->isList() || params[1]->isNil()) {
            auto vec = params[1]->toVector();
            std::vector<ValuePtr> args;
            for (int i = 0; i < vec.size(); i++) {
                if (vec[i]->isList()) {
                    args.push_back(env.eval(std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft()));
                }
                else if (!vec[i]->isNil()) {
                    args.push_back(env.eval(vec[i]));
                }
            }
            return std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()(args, env);
        }
        else {
            throw LispError("expected a list that can be applied on a procedure");
        }
    }
    else if (params[0]->getType() == ValueType::LAMBDA_VALUE) {
        if (params[1]->isList()) {
            auto vec = params[1]->toVector();
            std::vector<ValuePtr> args;
            for (int i = 0; i < vec.size(); i++) {
                if (vec[i]->isList()) {
                    args.push_back(env.eval(std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft()));
                }
                else if (!vec[i]->isNil()) {
                    args.push_back(env.eval(vec[i]));
                }
            }
            return std::dynamic_pointer_cast<LambdaValue>(params[0])->apply(args);
        }
        else {
            throw LispError("exprceted a list that can be applied on a procecure");
        }
    }
    else {
        throw LispError("expected a procedure that can be applied to arguments");
    }
}

ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) {
        throw LispError("Need a param.");
    }
    if (params.size() > 1) {
        throw LispError("Params should be only 1.");
    }
    auto param = env.eval(params[0]);
    if (param->getType() == ValueType::STRING_VALUE) {
        std::cout << std::dynamic_pointer_cast<StringValue>(params[0])->getValue() << std::endl;
    }
    else {
        std::cout << env.eval(param)->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

std::unordered_map<std::string, BuiltinFuncType*> innerSymbolTable{
    {"+", &add},
    {"print", &print}, 
    {"*", &multiply}, 
    {">", &larger}, 
    {"-", &reduce}, 
    {"apply", &apply}, 
    {"display", &display}
};