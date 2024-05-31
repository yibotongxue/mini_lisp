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

extern std::unordered_map<std::string, BuiltinFunction> innerSymbolTable;

#endif