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
    if (args.size() <= 2) {
        throw LispError("Nothing found to define " + *args[1]->asSymbol());
    }
    if (args.size() == 3 && args.back()->isNil()) {
        throw LispError("Nothing found to define " + *args[1]->asSymbol());
    }
    if (auto name = args[1]->asSymbol()) { // 定义变量
        auto first_Name = *args[1]->asSymbol();

        auto second_Value = args[2];

        env.defineBinding(first_Name, env.eval(args[2]));
        
        if (env.lookupBinding(first_Name)->getType() == ValueType::BUILTIN_PROC_VALUE) {
            innerSymbolTable.insert(std::make_pair(first_Name, std::dynamic_pointer_cast<BuiltinProcValue>(env.eval(std::make_shared<SymbolValue>(first_Name)))->getFunction()));
        }

        return std::make_shared<NilValue>();
    }
    else if (std::dynamic_pointer_cast<PairValue>(args[1])->isList()) { // 定义函数
        if (args[1]->isList()) {
            auto functionName = std::dynamic_pointer_cast<PairValue>(args[1])->getLeft();
            std::vector<std::string> params{};
            if (functionName->isSymbol()) {
                auto paramPtrs = args[1]->toVector();
                for (int i = 1; i < static_cast<int>(paramPtrs.size()) - 1; i++) {
                    if (paramPtrs[i]->isSymbol()){
                        params.push_back(*paramPtrs[i]->asSymbol());
                    }
                }
                std::vector<ValuePtr> functionBody{args.begin() + 2, args.end() - 1};
                env.defineBinding(*functionName->asSymbol(), std::make_shared<LambdaValue>(params, functionBody, env.shared_from_this()));
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
    return args[1];
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
    else if (args.size() > 5) {
        throw LispError("More sentence than expected.");
    }
    if (change_to_bool(args[1], env)) {
        return env.eval(args[2]);
    }
    else {
        if (args[3]->isNil())
            return args[3];
        return env.eval(args[3]);
    }
}

ValuePtr andForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    for (int i = 1; i < static_cast<int>(args.size()) - 1; i++) {
        if (!change_to_bool(args[i], env))
            return std::make_shared<BooleanValue>(false);
    }
    if (args.size() == 2)
        return std::make_shared<BooleanValue>(true);
    else
        return env.eval(args[args.size() - 2]);
}

ValuePtr orForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    for (int i = 1; i < static_cast<int>(args.size()) - 1; i++) {
        if (change_to_bool(args[i], env))
            return env.eval(args[i]);
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
        auto paramsPtr = args[1]->toVector();
        std::vector<std::string> params{};
        for (int i = 0; i < static_cast<int>(paramsPtr.size()) - 1; i++) {
            if (paramsPtr[i]->isSymbol()) {
                params.push_back(*paramsPtr[i]->asSymbol());
            }
            else {
                throw LispError("Not symbol in params.");
            }
        }
        std::vector<ValuePtr> body{};
        for (int i = 2; i < static_cast<int>(args.size()) - 1; i++) {
            body.push_back(args[i]);
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
            auto new_args = args[i]->toVector();
            if (new_args.size() <= 1) {
                throw LispError("There must be at least one sentence in cond.");
            }
            auto condition = new_args[0];
            std::vector<ValuePtr> results{};
            if (condition->isSymbol() && *condition->asSymbol() == "else") {
                if (i == args.size() - 2) {
                    for (int i = 1; i < static_cast<int>(new_args.size()) - 1; i++) {
                        results.push_back(env.eval(new_args[1]));
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
                        results.push_back(env.eval(new_args[i]));
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
        if (i == args.size() - 2) {
            return env.eval(args[i]);
        }
        else {
            env.eval(args[i]);
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
            auto params_args = args[1]->toVector();
            for (int i = 0; i < static_cast<int>(params_args.size()) - 1; i++) {
                if (params_args[i]->isList()) {
                    auto vec = params_args[i]->toVector();
                    if (vec.size() <= 2) {
                        throw LispError("The param name or value is missing.");
                    }
                    else if (vec.size() >= 3) {
                        if (vec[0]->isSymbol()) {
                            params.push_back(*vec[0]->asSymbol());
                        }
                        else {
                            throw LispError("The param name should be a symbol type.");
                        }
                        new_args.push_back(env.eval(vec[1]));
                    }
                    else {
                        throw LispError("The param list need param name and value, given 3 object.");
                    }
                }
                else {
                    throw LispError("The let form here need a list.");
                }
            }
            std::vector<ValuePtr> body{args.begin() + 2, args.end() - 1};
            auto lambda = std::make_shared<LambdaValue>(params, body, env.shared_from_this());
            return lambda->apply(new_args);
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

ValuePtr quasiquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() <= 2) {
        throw LispError("The quasiquote form need a arg.");
    }
    else if (args.size() == 3) {
        auto vec = args[1]->toVector();
        std::vector<ValuePtr> new_vec{};
        if (vec.size() == 0) {
            return std::make_shared<NilValue>();
        }
        else {
            for (int i = 0; i < static_cast<int>(vec.size()) - 1; i++) {
                    auto vecVec = vec[i]->toVector();
                    if (vecVec.size() == 3 && vecVec[0]->isSymbol() && *vecVec[0]->asSymbol() == "unquote") {
                        new_vec.push_back(env.eval(vecVec[1]));
                    }
                    else {
                        new_vec.push_back(vec[i]);
                    }
            }
            new_vec.push_back(std::make_shared<NilValue>());
            return makeList(new_vec, 0);
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
    {"quasiquote", quasiquoteForm}
};
