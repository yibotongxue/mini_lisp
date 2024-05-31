/* ************************************************************************
> File Name:     forms.cpp
> Author:        林毅波
> Created Time:  2024/5/26 14:04:26
> Description:   
 ************************************************************************/

#include "../include/forms.h"
#include "../include/error.h"
#include "../include/builtins.h"
#include <iostream>

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 2) {
        throw LispError("Nothing found to define " + *args[1]->asSymbol());
    }
    if (args.size() == 3 && args.back()->isNil()) {
        throw LispError("Nothing found to define " + *args[1]->asSymbol());
    }
    if (auto name = std::dynamic_pointer_cast<PairValue>(args[1])->getLeft()->asSymbol()) {
        auto first_Name = std::dynamic_pointer_cast<SymbolValue>(std::dynamic_pointer_cast<PairValue>(args[1])->getLeft())->getName();

        auto second_Value = args[2];

        if (second_Value->isList() && std::dynamic_pointer_cast<PairValue>(second_Value)->getRight()->isNil()) {
            env.defineBinding(first_Name, env.eval(std::dynamic_pointer_cast<PairValue>(second_Value)->getLeft()));
        }
        else {
            env.defineBinding(first_Name, env.eval(second_Value));
        }

        if (env.lookupBinding(first_Name)->getType() == ValueType::BUILTIN_PROC_VALUE) {
            innerSymbolTable.insert(std::make_pair(first_Name, std::dynamic_pointer_cast<BuiltinProcValue>(env.eval(std::make_shared<SymbolValue>(first_Name)))->getFunction()));
        }

        return std::make_shared<NilValue>();
    }
    else if (std::dynamic_pointer_cast<PairValue>(args[1])->isList()) {
        auto Pairptr = std::dynamic_pointer_cast<PairValue>(args[1]);
        auto functionName_params = Pairptr->getLeft();
        if (functionName_params->isList()) {
            auto functionName = std::dynamic_pointer_cast<PairValue>(functionName_params)->getLeft();
            std::vector<std::string> params{};\
            if (functionName->isSymbol()) {
                auto paramPtrs = functionName_params->toVector();
                for (int i = 1; i < paramPtrs.size() - 1; i++) {
                    if (paramPtrs[i]->isList()) {
                        auto param = std::dynamic_pointer_cast<PairValue>(paramPtrs[i])->getLeft();
                        if (param->isSymbol()) {
                            params.push_back(*param->asSymbol());
                        }
                    }
                }
                auto functionBodyPtr = Pairptr->toVector();
                std::vector<ValuePtr> functionBody{};
                for (int i = 1; i < functionBodyPtr.size() - 1; i++) {
                    functionBody.push_back(std::dynamic_pointer_cast<PairValue>(functionBodyPtr[i])->getLeft());
                }
                env.defineBinding(*functionName->asSymbol(), std::make_shared<LambdaValue>(params, functionBody, env.shared_from_this()));
                // env.symbolList[*functionName->asSymbol()] = std::make_shared<LambdaValue>(params, functionBody);
                return std::make_shared<NilValue>();
            }
            else {
                throw LispError("Malformed define.");
            }
        }
        else {
            throw LispError("Malformed define.");
        }
    }
    else {
        throw LispError("Malformed define.");
    }
}

ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 3)
        throw LispError("Nothing to quoted.");
    return std::dynamic_pointer_cast<PairValue>(args[1])->getLeft();
}

namespace{
    bool change_to_bool(ValuePtr ptr, EvalEnv& env) {
        if (ptr->isNil()) {
            return false;
        }
        else if (ptr->getType() == ValueType::BOOLEAN_VALUE) {
            return std::dynamic_pointer_cast<BooleanValue>(ptr)->getValue();
        }
        else if (ptr->getType() == ValueType::NUMERIC_VALUE) {
            return std::dynamic_pointer_cast<NumericValue>(ptr)->getValue();
        }
        else if (ptr->getType() == ValueType::STRING_VALUE) {
            return std::dynamic_pointer_cast<StringValue>(ptr)->getValue() != "";
        }
        else if (ptr->isSymbol()) {
            return change_to_bool(env.eval(ptr), env);
        }
        else if (ptr->isList()) {
            auto pairPtr = std::dynamic_pointer_cast<PairValue>(ptr);
            if (pairPtr->getLeft()->isSymbol() && *pairPtr->getLeft()->asSymbol() == "quote") {
                return true;
            }
        }
        return change_to_bool(env.eval(ptr), env);
    }
}

ValuePtr ifForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    if (args.size() == 2) {
        throw LispError("No conditional statements are defined");
    }
    else if (args.size() == 3) {
        auto temp = args[1];
        std::vector<ValuePtr> new_args{args};
        new_args.pop_back();
        new_args.push_back(std::dynamic_pointer_cast<PairValue>(temp)->getRight());
        if (new_args.back()->isNil()) {
            throw LispError("There is no true branch defined");
        }
        new_args.push_back(std::make_shared<NilValue>());
        return ifForm(new_args, env);
    }
    else if (args.size() == 4) {
        auto temp = args[2];
        std::vector<ValuePtr> new_args{args};
        new_args.pop_back();
        new_args.push_back(std::dynamic_pointer_cast<PairValue>(temp)->getRight());
        if (new_args.back()->isNil()) {
            throw LispError("There is no false branch defined");
        }
        new_args.push_back(std::make_shared<NilValue>());
        return ifForm(new_args, env);
    }
    else if (args.size() > 5) {
        throw LispError("More sentence than expected.");
    }
    auto condiction = std::dynamic_pointer_cast<PairValue>(args[1])->getLeft();
    if (change_to_bool(condiction, env)) {
        return env.eval(std::dynamic_pointer_cast<PairValue>(args[2])->getLeft());
    }
    else {
        return env.eval(std::dynamic_pointer_cast<PairValue>(args[3])->getLeft());
    }
}

ValuePtr andForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    for (int i = 1; i < args.size() - 1; i++) {
        if (!change_to_bool(std::dynamic_pointer_cast<PairValue>(args[i])->getLeft(), env))
            return std::make_shared<BooleanValue>(false);
    }
    if (args.size() == 2)
        return std::make_shared<BooleanValue>(true);
    else
        return env.eval(std::dynamic_pointer_cast<PairValue>(args[args.size() - 2])->getLeft());
}

ValuePtr orForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    for (int i = 1; i < args.size() - 1; i++) {
        if (change_to_bool(std::dynamic_pointer_cast<PairValue>(args[i])->getLeft(), env))
            return env.eval(std::dynamic_pointer_cast<PairValue>(args[i])->getLeft());
    }
    return std::make_shared<BooleanValue>(false);
}

ValuePtr lambdaForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    if (args.size() == 2) {
        throw LispError("The params are missing.");
    }
    else if (args.size() == 3) {
        throw LispError("The name of the process body is missing.");
    }
    else {
        auto paramsPtr = std::dynamic_pointer_cast<PairValue>(args[1])->getLeft()->toVector();
        std::vector<std::string> params{};
        for (auto& param : paramsPtr) {
            if (param->isList()) {
                auto paramPtr = std::dynamic_pointer_cast<PairValue>(param)->getLeft();
                if (paramPtr->isSymbol()) {
                    params.push_back(*paramPtr->asSymbol());
                }
                else {
                    throw LispError("Not symbol in params.");
                }
            }
            else if (param->isSymbol()) {
                params.push_back(*param->asSymbol());
            }
        }
        auto bodyPtr = args[1]->toVector();
        std::vector<ValuePtr> body{};
        for (int i = 1; i < bodyPtr.size() - 1; i++) {
            body.push_back(std::dynamic_pointer_cast<PairValue>(bodyPtr[i])->getLeft());
        }
        return std::make_shared<LambdaValue>(params, body, env.shared_from_this());
    }
}

std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", defineForm},
    {"quote", quoteForm},
    {"if", ifForm},
    {"and", andForm},
    {"or", orForm}, 
    {"lambda", lambdaForm}
};
