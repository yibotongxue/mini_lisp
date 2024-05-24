/* ************************************************************************
> File Name:     builtins.cpp
> Author:        林毅波
> Created Time:  2024/5/23 21:26:41
> Description:   
 ************************************************************************/

#include "..\include\builtins.h"
#include "../include/error.h"

ValuePtr add(const std::vector<ValuePtr>& params) {
    double result = 0;
    for (const auto& i :params) {
        if(i->getType() != ValueType::NUMERIC_VALUE) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += std::dynamic_pointer_cast<NumericValue>(i)->getValue();
    }
    return std::make_shared<NumericValue>(result);
}