/* ************************************************************************
> File Name:     forms.cpp
> Author:        林毅波
> Created Time:  2024/5/26 14:04:26
> Description:   
 ************************************************************************/

#include "../include/forms.h"
#include "../include/error.h"

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
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
        }

        return std::make_shared<NilValue>();
    }
    else {
        throw LispError("Malformed define.");
    }
}

std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", defineForm}
};
