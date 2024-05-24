/* ************************************************************************
> File Name:     eval_env.h
> Author:        林毅波
> Created Time:  2024/5/17 15:47:39
> Description:   
 ************************************************************************/

#include "./value.h"
#include "./builtins.h"
#include <unordered_map>
#include <algorithm>
#include <iterator>

/**
 * @brief EvalEnv 类用于表示表达式的求值环境
 * 
 * EvalEnv 类包含一个私有成员 symbolList，用于存储符号和对应的值的映射关系。
 */
class EvalEnv {
private:
    std::unordered_map<std::string, ValuePtr> symbolList; // 用于存储符号和值对应关系的无序映射

public:
    EvalEnv();

    /**
     * @brief 对给定的表达式进行求值
     * 
     * @param expr 要求值的表达式的指针
     * @return 求值结果的指针
    */
    ValuePtr eval(ValuePtr expr);

    std::vector<ValuePtr> evalList(ValuePtr expr);

    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr>& args);
};
