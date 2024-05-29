/* ************************************************************************
> File Name:     builtins.cpp
> Author:        林毅波
> Created Time:  2024/5/23 21:26:41
> Description:   
 ************************************************************************/

#include "..\include\builtins.h"
#include "../include/error.h"
#include <iostream>

ValuePtr add(const std::vector<ValuePtr>& params) {
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

ValuePtr print(const std::vector<ValuePtr>& params) {
    for (const auto& param : params) {
        std::cout << param->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

ValuePtr multiply(const std::vector<ValuePtr>& params) {
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

ValuePtr larger(const std::vector<ValuePtr>& params) {
    if (params.size() < 2) {
        throw LispError("Less params than needed.");
    }
    else {
        bool result = true;
        if (!params[0]->isNumber()) {
            throw LispError("Cannot compare a non-numeric value.");
        }
        for (int i = 2; i < params.size() - 1; i++) {
            if (params[i]->isNumber()) {
                result = result && *params[i]->asNumber() > *params[i - 1]->asNumber();
            }
            else {
                throw LispError("Cannot compare a non-numeric value.");
            }
        }
        return std::make_shared<BooleanValue>(result);
    }
}

std::unordered_map<std::string, BuiltinFuncType*> innerSymbolTable{
    {"+", &add},
    {"print", &print}, 
    {"*", &multiply}, 
    {">", &larger}
};