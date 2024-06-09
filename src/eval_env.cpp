/* ************************************************************************
> File Name:     eval_env.cpp
> Author:        林毅波
> Created Time:  2024/5/17 15:49:20
> Description:   
 ************************************************************************/
#include "./eval_env.h"
#include "error.h"
#include "forms.h"
#include "builtins.h"
#include <iostream>

using namespace std::literals;

/**
 * @brief 默认构造函数，创建一个空的求值环境。
 */
EvalEnv::EvalEnv() : symbolList{}, parent{nullptr} {
    for (auto& item : innerSymbolTable) {
        symbolList.insert(std::make_pair(item.first, std::make_shared<BuiltinProcValue>(item.second)));
    }
}

/**
 * @brief 构造函数，创建一个带有父环境的求值环境。
 * 
 * @param ptr 父环境的指针
 */
EvalEnv::EvalEnv(std::shared_ptr<EvalEnv>& ptr) : symbolList{}, parent{ptr} {
    for (auto& item : innerSymbolTable) {
        symbolList.insert(std::make_pair(item.first, std::make_shared<BuiltinProcValue>(item.second)));
    }
}

/**
 * @brief 创建一个新的求值环境。
 * 
 * @return 新的求值环境对象
 */
std::shared_ptr<EvalEnv> EvalEnv::createEvalEnv() {
    return std::make_shared<EvalEnv>(std::move(EvalEnv()));
}

/**
 * @brief 创建一个带有父环境的新的求值环境。
 * 
 * @param ptr 父环境的指针
 * @return 新的求值环境对象
 */
std::shared_ptr<EvalEnv> EvalEnv::createEvalEnv(std::shared_ptr<EvalEnv>& ptr)  {
    EvalEnv env(ptr);
    return std::make_shared<EvalEnv>(std::move(EvalEnv(ptr)));
}

/**
 * @brief 对给定的表达式进行求值
 * 
 * @param expr 要求值的表达式
 * @return 求值结果的指针
 * @throw LispError 如果求值过程中出现错误，则抛出异常
 */
ValuePtr EvalEnv::eval(ValuePtr expr) {
    // 检查表达式的类型并执行相应操作

    if (expr->isSelfEvaluating()) {
        // 如果表达式是自求值类型，则直接返回
        return expr;
    }
    else if (expr->isNil()) {
        // 禁止对空表进行评估，抛出异常
        throw LispError("Evaluating nil is prohibited.");
    }
    else if (expr->isList() && !expr->isNil()) {
        // 如果表达式是列表类型，则解析列表并执行相应操作
        
        // 解析表达式为向量
        std::vector<ValuePtr> v = expr->toVector();

        if (v[0]->asSymbol()) {
            auto name = *v[0]->asSymbol();
            if (SPECIAL_FORMS.find(name) != SPECIAL_FORMS.end()) {
                return SPECIAL_FORMS[name](v, *this);
            }
            else {
                auto proc = eval(v[0]);
                std::vector<ValuePtr> args = evalList(expr);
                return apply(proc, args);
            }
        }
        else if (v[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
            return std::dynamic_pointer_cast<BuiltinProcValue>(v[0])->getFunction()(evalList(expr), *this);
        }
        else if (v[0]->getType() == ValueType::LAMBDA_VALUE) {
            return std::dynamic_pointer_cast<LambdaValue>(v[0])->apply(evalList(expr));
        }
        else if (v[0]->isSelfEvaluating()) {
            throw LispError("Unimplement in eval_env.cpp, line 78");
        }
        else {
            return eval(std::make_shared<PairValue>(eval(v[0]), std::dynamic_pointer_cast<PairValue>(expr)->getRight()));
        }
    }
    else if (expr->isSymbol()) {
        // 如果表达式是符号类型，则查找并返回符号对应的值

        auto name = std::dynamic_pointer_cast<SymbolValue>(expr)->getName();
        return lookupBinding(name);
    }
    else {
        // 其他类型的表达式暂未实现，抛出异常
        throw LispError("Unimplement in eval_env.cpp, line 124");
    }
}

/**
 * @brief 对表达式列表进行求值。
 * 
 * @param expr 表达式
 * @return 求值结果的向量
 */
std::vector<ValuePtr>EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::vector<ValuePtr> vec = expr->toVector();
    if (vec.empty())
        return {};
    for (int i = 1; i < vec.size() - 1; i++) {
        result.push_back(eval(vec[i]));
    }
    return result;
}

/**
 * @brief 应用过程，执行函数或内置过程。
 * 
 * @param proc 过程对象
 * @param args 参数列表
 * @return 执行结果
 */
ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr>& args) {
    if (proc->getType() == ValueType::BUILTIN_PROC_VALUE) {
        return std::dynamic_pointer_cast<BuiltinProcValue>(proc)->getFunction()(args, *this);
    }
    else if (proc->getType() == ValueType::LAMBDA_VALUE) {
        return std::dynamic_pointer_cast<LambdaValue>(proc)->apply(args);
    }
    else {
        throw LispError("Unimplemented in eval_env.cpp, line 157");
    }
}

/**
 * @brief 查找绑定的值。
 * 
 * @param name 符号名称
 * @return 绑定的值
 */
ValuePtr EvalEnv::lookupBinding(const std::string& name) {
    if (symbolList.find(name) != symbolList.end()) {
        return symbolList[name];
    }
    if (parent) 
        return parent->lookupBinding(name);
    else 
        throw LispError("Variable " + name + " not defined.");
}

/**
 * @brief 定义绑定。
 * 
 * @param name 符号名称
 * @param ptr 值对象指针
 * @return 定义的值
 */
ValuePtr EvalEnv::defineBinding(const std::string& name, ValuePtr ptr) {
    symbolList[name] = ptr;
    if (parent)
        return parent->defineBinding(name, ptr);
    return std::make_shared<NilValue>();
}

/**
 * @brief 创建子环境。
 * 
 * @param params 参数列表
 * @param args 参数值列表
 * @return 子环境对象
 */
std::shared_ptr<EvalEnv> EvalEnv::createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args) {
    auto env = EvalEnv::createEvalEnv();
    if (params.size() < args.size()) {
        throw LispError("The params are more than needed.");
    }
    else if (params.size() > args.size()) {
        throw LispError("The params are less than needed.");
    }
    for (int i = 0; i < params.size(); i++) {
        env->symbolList[params[i]] = args[i];
    }
    env->parent = std::make_shared<EvalEnv>(*this);
    return env;
}
