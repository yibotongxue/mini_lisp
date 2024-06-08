/* ************************************************************************
> File Name:     builtins.h
> Author:        林毅波
> Created Time:  2024/5/23 21:25:09
> Description:   
 ************************************************************************/
#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include "../include/value.h"
#include <unordered_map>

// 储存内置运算符及其对应的函数的哈希表，键为运算符的字符串，值为对应的函数指针，函数类型为 ValuePtr(const std::vector<ValuePtr>&, EvalEnv&)
extern std::unordered_map<std::string, BuiltinFuncType*> innerSymbolTable;

#endif