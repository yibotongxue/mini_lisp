/* ************************************************************************
> File Name:     builtins.h
> Author:        林毅波
> Created Time:  2024/5/23 21:25:09
> Description:   
 ************************************************************************/
#ifndef __BUILTINS_H__
#define __BUILTINS_H_

#include "../include/value.h"

ValuePtr add(const std::vector<ValuePtr>& params);

ValuePtr print(const std::vector<ValuePtr>& params);

#endif