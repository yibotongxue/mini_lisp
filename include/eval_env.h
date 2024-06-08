/* ************************************************************************
> File Name:     eval_env.h
> Author:        林毅波
> Created Time:  2024/5/17 15:47:39
> Description:   
 ************************************************************************/
#ifndef __EVAL_ENV_H__
#define __EVAL_ENV_H__


#include "./value.h"
#include "./builtins.h"
#include <unordered_map>
#include <algorithm>
#include <iterator>

/**
 * @brief EvalEnv类表示Mini-Lisp中的求值环境。
 * 
 * 求值环境是一个关联符号和值的表，用于存储变量和函数定义。EvalEnv类维护了一个
 * 符号表，其中包含了内置过程（Builtin Procedure）的映射关系。它还可以通过父环境
 * 进行嵌套，以支持作用域链的查找。
 */
class EvalEnv : public std::enable_shared_from_this<EvalEnv> {
private:
    std::shared_ptr<EvalEnv> parent; // 环境的上级环境，空指针表示这是初始环境

    std::unordered_map<std::string, ValuePtr> symbolList; // 用于存储符号和值对应关系的无序映射
    
    /**
     * @brief 默认构造函数，创建一个空的求值环境。
     */
    EvalEnv();

    /**
     * @brief 构造函数，创建一个带有父环境的求值环境。
     * 
     * @param ptr 父环境的指针
     */
    EvalEnv(std::shared_ptr<EvalEnv>& ptr);

public:
    /**
     * @brief 创建一个新的求值环境。
     * 
     * @return 新的求值环境对象
     */
    static std::shared_ptr<EvalEnv> createEvalEnv();

    /**
     * @brief 创建一个带有父环境的新的求值环境。
     * 
     * @param ptr 父环境的指针
     * @return 新的求值环境对象
     */
    static std::shared_ptr<EvalEnv> createEvalEnv(std::shared_ptr<EvalEnv>& ptr);

    /**
     * @brief 对给定的表达式进行求值
     * 
     * @param expr 要求值的表达式的指针
     * @return 求值结果的指针
    */
    ValuePtr eval(ValuePtr expr);

    /**
     * @brief 对表达式列表进行求值。
     * 
     * @param expr 表达式
     * @return 求值结果的向量
     */
    std::vector<ValuePtr> evalList(ValuePtr expr);

    /**
     * @brief 应用过程，执行函数或内置过程。
     * 
     * @param proc 过程对象
     * @param args 参数列表
     * @return 执行结果
     */
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr>& args);

    /**
     * @brief 查找绑定的值。
     * 
     * @param name 符号名称
     * @return 绑定的值
     */
    ValuePtr lookupBinding(const std::string& name);

    /**
     * @brief 定义绑定。
     * 
     * @param name 符号名称
     * @param ptr 值对象指针
     * @return 定义的值
     */
    ValuePtr defineBinding(const std::string& name, ValuePtr ptr);

    /**
     * @brief 创建子环境。
     * 
     * @param params 参数列表
     * @param args 参数值列表
     * @return 子环境对象
     */
    std::shared_ptr<EvalEnv> createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args);
};

#endif