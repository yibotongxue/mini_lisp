/* ************************************************************************
> File Name:     forms.cpp
> Author:        林毅波
> Created Time:  2024/5/26 14:04:26
> Description:   
 ************************************************************************/

#include "../include/forms.h"
#include "../include/error.h"
#include "../include/builtins.h"

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 3 && args.back()->isNil()) {
        throw LispError("Nothing found to define " + *args[1]->asSymbol());
    }
    if (auto name = args[1]->asSymbol()) {
        auto first_Name = std::dynamic_pointer_cast<SymbolValue>(args[1])->getName();

        auto second_Value = args[2];

        if (second_Value->isList() && std::dynamic_pointer_cast<PairValue>(second_Value)->getRight()->isNil()) {
            env.symbolList[first_Name] = env.eval(std::dynamic_pointer_cast<PairValue>(second_Value)->getLeft());
        }
        else {
            env.symbolList[first_Name] = env.eval(second_Value);
        }

        if (env.symbolList[first_Name]->getType() == ValueType::BUILTIN_PROC_VALUE) {
            innerSymbolTable.insert(std::make_pair(first_Name, std::dynamic_pointer_cast<BuiltinProcValue>(env.symbolList[first_Name])->getFunction()));
        }

        return std::make_shared<NilValue>();
    }
    else {
        throw LispError("Malformed define.");
    }
}

ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 3)
        throw LispError("Nothing to quoted.");
    return args[1];
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
        else {
            env.eval(ptr);
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
    auto condiction = args[1];
    if (change_to_bool(condiction, env)) {
        return env.eval(args[2]);
    }
    else {
        return env.eval(args[3]);
    }
}

ValuePtr andForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    for (int i = 1; i < args.size() - 1; i++) {
        if (!change_to_bool(args[i], env))
            return std::make_shared<BooleanValue>(false);
    }
    if (args.size() == 2)
        return std::make_shared<BooleanValue>(true);
    else
        return args[args.size() - 2];
}

ValuePtr orForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    for (int i = 1; i < args.size() - 1; i++) {
        if (change_to_bool(args[i], env))
            return args[i];
    }
    return std::make_shared<BooleanValue>(false);
}

std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", defineForm},
    {"quote", quoteForm},
    {"if", ifForm},
    {"and", andForm},
    {"or", orForm}
};
