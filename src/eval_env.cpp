/* ************************************************************************
> File Name:     eval_env.cpp
> Author:        林毅波
> Created Time:  2024/5/17 15:49:20
> Description:   
 ************************************************************************/
#include "../include/eval_env.h"
#include "../include/error.h"
#include "../include/forms.h"
#include "../include/builtins.h"
#include <iostream>

using namespace std::literals;

EvalEnv::EvalEnv() : symbolList{}, parent{nullptr} {
    for (auto& item : innerSymbolTable) {
        symbolList.insert(std::make_pair(item.first, std::make_shared<BuiltinProcValue>(item.second)));
    }
}

EvalEnv::EvalEnv(std::shared_ptr<EvalEnv>& ptr) : symbolList{}, parent{ptr} {
    for (auto& item : innerSymbolTable) {
        symbolList.insert(std::make_pair(item.first, std::make_shared<BuiltinProcValue>(item.second)));
    }
}

std::shared_ptr<EvalEnv> EvalEnv::createEvalEnv() {
    return std::make_shared<EvalEnv>(std::move(EvalEnv()));
}

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
                std::vector<ValuePtr> args = evalList(v[1]);
                return apply(proc, args);
            }
        }
        else if (v[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
            return std::dynamic_pointer_cast<BuiltinProcValue>(v[0])->getFunction()(evalList(v[1]), *this);
        }
        else if (v[0]->getType() == ValueType::LAMBDA_VALUE) {

            return std::dynamic_pointer_cast<LambdaValue>(v[0])->apply(evalList(v[1]));
        }
        else if (v[0]->isSelfEvaluating()) {
            throw LispError("Unimplement in eval_env.cpp, line 78");
        }
        else {
            return eval(std::make_shared<PairValue>(eval(v[0]), std::dynamic_pointer_cast<PairValue>(expr)->getRight()));
        }
        // else {
        //     throw LispError("Unimplement");
        // }

        // // 根据列表的首元素执行不同的操作
        // if (v[0]->asSymbol() == "define"s) {
        //     // 如果是 define 操作符，则进行定义操作
        //     if (auto name = v[1]->asSymbol()) {
        //         // 获取定义名称
        //         auto first_Name = std::dynamic_pointer_cast<SymbolValue>(v[1])->getName();

        //         auto second_Value = v[2];

        //         if (second_Value->isList() && std::dynamic_pointer_cast<PairValue>(second_Value)->getRight()->isNil()) {
        //             symbolList[first_Name] = eval(std::dynamic_pointer_cast<PairValue>(second_Value)->getLeft());
        //         }
        //         else {
        //             symbolList[first_Name] = eval(second_Value);
        //         }

        //         if(symbolList[first_Name]->getType() == ValueType::BUILTIN_PROC_VALUE) {
        //             pairParser.add(first_Name);
        //         }

        //         // 返回空表表示成功
        //         return std::make_shared<NilValue>();
        //     }
        //     else {
        //         // define 操作符形式错误，抛出异常
        //         throw LispError("Malformed define.");
        //     }
        // }
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

std::vector<ValuePtr>EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::vector<ValuePtr> vec = expr->toVector();
    if (vec.empty())
        return {};
    result.push_back(eval(vec[0]));
    for(int i = 1; i < vec.size(); i++) {
        if (vec[i]->isList()) {
            auto pairPtr = std::dynamic_pointer_cast<PairValue>(vec[i]);
            if (pairPtr->getLeft()->isSymbol()) {
                auto name = *pairPtr->getLeft()->asSymbol();
            }
            result.push_back(eval(std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft()));
        }
        else if (!vec[i]->isNil()) {
            result.push_back(eval(vec[i]));
        }
    }
    return result;
}

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

ValuePtr EvalEnv::lookupBinding(const std::string& name) {
    if (symbolList.find(name) != symbolList.end()) {
        return symbolList[name];
    }
    if (parent) 
        return parent->lookupBinding(name);
    else 
        throw LispError("Variable " + name + " not defined.");
}

ValuePtr EvalEnv::defineBinding(const std::string& name, ValuePtr ptr) {
    symbolList[name] = ptr;
    if (parent)
        return parent->defineBinding(name, ptr);
    return std::make_shared<NilValue>();
}

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
