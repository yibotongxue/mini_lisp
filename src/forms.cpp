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
    if (auto name = std::dynamic_pointer_cast<PairValue>(args[1])->getLeft()->asSymbol()) { // 定义变量
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
    else if (std::dynamic_pointer_cast<PairValue>(args[1])->isList()) { // 定义函数
        auto Pairptr = std::dynamic_pointer_cast<PairValue>(args[1]);
        auto functionName_params = Pairptr->getLeft();
        if (functionName_params->isList()) {
            auto functionName = std::dynamic_pointer_cast<PairValue>(functionName_params)->getLeft();
            std::vector<std::string> params{};\
            if (functionName->isSymbol()) {
                auto paramPtrs = functionName_params->toVector();
                for (int i = 1; i < static_cast<int>(paramPtrs.size()) - 1; i++) {
                    if (paramPtrs[i]->isList()) {
                        auto param = std::dynamic_pointer_cast<PairValue>(paramPtrs[i])->getLeft();
                        if (param->isSymbol()) {
                            params.push_back(*param->asSymbol());
                        }
                    }
                }
                auto functionBodyPtr = Pairptr->toVector();
                std::vector<ValuePtr> functionBody{};
                for (int i = 1; i < static_cast<int>(functionBodyPtr.size()) - 1; i++) {
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
    if (args.size() < 3) // quote特殊形式、引用内容、空表
        throw LispError("Nothing to quoted.");
    return std::dynamic_pointer_cast<PairValue>(args[1])->getLeft();
}

namespace{
    bool change_to_bool(ValuePtr ptr, EvalEnv& env) {
        if (env.eval(ptr)->getType() == ValueType::BOOLEAN_VALUE && !std::dynamic_pointer_cast<BooleanValue>(env.eval(ptr))->getValue()) {
            return false;
        }
        else {
            return true;
        }
    }
}

ValuePtr ifForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    if (args.size() == 2) {
        throw LispError("No conditional statements are defined");
    }
    else if (args.size() == 3) {
        throw LispError("There is no true branch defined");
    }
    else if (args.size() == 4) {
        throw LispError("There is no false branch defined");
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
    for (int i = 1; i < static_cast<int>(args.size()) - 1; i++) {
        if (!change_to_bool(std::dynamic_pointer_cast<PairValue>(args[i])->getLeft(), env))
            return std::make_shared<BooleanValue>(false);
    }
    if (args.size() == 2)
        return std::make_shared<BooleanValue>(true);
    else
        return env.eval(std::dynamic_pointer_cast<PairValue>(args[args.size() - 2])->getLeft());
}

ValuePtr orForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    for (int i = 1; i < static_cast<int>(args.size()) - 1; i++) {
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
        for (int i = 1; i < static_cast<int>(bodyPtr.size()) - 1; i++) {
            body.push_back(std::dynamic_pointer_cast<PairValue>(bodyPtr[i])->getLeft());
        }
        return std::make_shared<LambdaValue>(params, body, env.shared_from_this());
    }
}

ValuePtr condForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() <= 2) { // cond 和 空表
        return std::make_shared<NilValue>();
    }
    else {
        ValuePtr* result = nullptr;
        for (int i = 1; i < static_cast<int>(args.size()) - 1; i++) {
            auto new_args = std::dynamic_pointer_cast<PairValue>(args[i])->getLeft()->toVector();
            if (new_args.size() <= 1) {
                throw LispError("There must be at least one sentence in cond.");
            }
            auto condition = new_args[0];
            std::vector<ValuePtr> results{};
            if (condition->isSymbol() && *condition->asSymbol() == "else") {
                if (i == args.size() - 2) {
                    for (int i = 1; i < static_cast<int>(new_args.size()) - 1; i++) {
                        auto arg = std::dynamic_pointer_cast<PairValue>(new_args[1])->getLeft();
                        results.push_back(env.eval(arg));
                    }
                    if (results.empty()) {
                        return std::make_shared<NilValue>();
                    }
                    else {
                        return results.back();
                    }
                }
                else {
                    throw LispError("THe else is not in the last.");
                }
            }
            else {
            results.push_back(env.eval(new_args[0]));
            if (change_to_bool(condition, env)) {
                    for (int i = 1; i < static_cast<int>(new_args.size()) - 1; i++) {
                        auto arg = std::dynamic_pointer_cast<PairValue>(new_args[i])->getLeft();
                        results.push_back(env.eval(arg));
                    }
                    return results.back();
                }
            }
        }
        return std::make_shared<NilValue>();
    }
}

ValuePtr beginForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2) {
        throw LispError("Error!");
    }
    for (int i = 1; i < static_cast<int>(args.size()) - 1; i++) {
        if (args[i]->isList()) {
            auto arg = std::dynamic_pointer_cast<PairValue>(args[i])->getLeft();
            if (i == args.size() - 2) {
                return env.eval(arg);
            }
            else {
                env.eval(arg);
            }
        }
    }
    return std::make_shared<NilValue>();
}

ValuePtr letForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() <= 2) {
        throw LispError("The let form need variable sentence.");
    }
    else if (args.size() == 3) {
        throw LispError("The let form need function body.");
    }
    else {
        std::vector<std::string> params{};
        std::vector<ValuePtr> new_args{};
        try {
            auto params_args = std::dynamic_pointer_cast<PairValue>(args[1])->getLeft()->toVector();
            for (int i = 0; i < static_cast<int>(params_args.size()) - 1; i++) {
                if (params_args[i]->isList()) {
                    auto vec = params_args[i]->toVector();
                    if (vec.size() <= 2) {
                        throw LispError("The param name or value is missing.");
                    }
                    else if (vec.size() == 3) {
                        if (vec[0]->isSymbol()) {
                            params.push_back(*vec[0]->asSymbol());
                        }
                        else {
                            throw LispError("The param name should be a symbol type.");
                        }
                        if (vec[1]->isList()) {
                            new_args.push_back(env.eval(std::dynamic_pointer_cast<PairValue>(vec[1])->getLeft()));
                        }
                    }
                    else {
                        throw LispError("The param list need param name and value, given 3 object.");
                    }
                }
                else {
                    throw LispError("The let form here need a list.");
                }
            }
            std::vector<ValuePtr> body{};
            for (int i = 2; i < static_cast<int>(args.size()) - 1; i++) {
                if (args[i]->isList()) {
                    body.push_back(std::dynamic_pointer_cast<PairValue>(args[i])->getLeft());
                }
            }
            auto lambda = std::make_shared<LambdaValue>(params, body, env.shared_from_this());
            return lambda->apply(new_args);
        }
        catch(...) {
            throw LispError("Unimplement error.");
        }
    }
}

namespace{
    ValuePtr makeList(std::vector<ValuePtr>& vec, int start) {
        if (vec.size() == 0) {
            return std::make_shared<NilValue>();
        }
        else if(vec.size() == 1) {
            return vec[0];
        }
        if (start == vec.size() - 2) {
            return std::make_shared<PairValue>(vec[start], vec[start + 1]);
        }
        else {
            return std::make_shared<PairValue>(vec[start], makeList(vec, start + 1));
        }
    }
}

ValuePtr quasiquoteteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() <= 2) {
        throw LispError("The quasiquote form need a arg.");
    }
    else if (args.size() == 3) {
        if (args[1]->isList()){
            auto arg = std::dynamic_pointer_cast<PairValue>(args[1])->getLeft();
            if (arg->isList()) {
                auto vec = arg->toVector();
                std::vector<ValuePtr> new_vec{};
                if (vec.size() == 0) {
                    return std::make_shared<NilValue>();
                }
                else {
                    auto vecVec = vec[0]->toVector();
                    if (vecVec.size() == 3 && vecVec[0]->isSymbol() && *vecVec[0]->asSymbol() == "unquote") {
                        if (vecVec[1]->isList()) {
                            auto ptr = std::dynamic_pointer_cast<PairValue>(vecVec[1])->getLeft();
                            new_vec.push_back(env.eval(ptr));
                        }
                    }
                    else {
                        new_vec.push_back(vec[0]);
                    }
                    for (int i = 1; i < static_cast<int>(vec.size()) - 1; i++) {
                        if (vec[i]->isList()) {
                            auto element = std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft();
                            auto vecVec = element->toVector();
                            if (vecVec.size() == 3 && vecVec[0]->isSymbol() && *vecVec[0]->asSymbol() == "unquote") {
                                if (vecVec[1]->isList()) {
                                    auto ptr = std::dynamic_pointer_cast<PairValue>(vecVec[1])->getLeft();
                                    new_vec.push_back(env.eval(ptr));
                                }
                            }
                            else {
                                new_vec.push_back(element);
                            }
                        }
                    }
                    new_vec.push_back(std::make_shared<NilValue>());
                    return makeList(new_vec, 0);
                }
            }
            else {
                return arg;
            }
        }
        else {
            throw LispError("Unimplement error.");
        }
    }
    else {
        throw LispError("The quasiquote form need 1 args.");
    }
}

std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", defineForm},
    {"quote", quoteForm},
    {"if", ifForm},
    {"and", andForm},
    {"or", orForm}, 
    {"lambda", lambdaForm}, 
    {"cond", condForm}, 
    {"begin", beginForm}, 
    {"let", letForm}, 
    {"quasiquote", quasiquoteteForm}
};
