/* ************************************************************************
> File Name:     eval_env.h
> Author:        林毅波
> Created Time:  2024/5/17 15:47:39
> Description:   
 ************************************************************************/

#include "./value.h"
#include <unordered_map>

class EvalEnv {
private:
    std::unordered_map<std::string, ValuePtr> symbolList;
public:
    ValuePtr eval(ValuePtr expr);
};
