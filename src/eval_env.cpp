/* ************************************************************************
> File Name:     eval_env.cpp
> Author:        林毅波
> Created Time:  2024/5/17 15:49:20
> Description:   
 ************************************************************************/
#include "../include/eval_env.h"
#include "../include/error.h"
#include "../include/pair_parser.h"

using namespace std::literals;

EvalEnv::EvalEnv() {
    symbolList.insert(std::make_pair("+", std::make_shared<BuiltinProcValue>(&add)));
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
    else if (expr->isList()) {
        // 如果表达式是列表类型，则解析列表并执行相应操作
        
        // 解析表达式为向量
        std::vector<ValuePtr> v = PairParser(expr).parse();

        

        // 根据列表的首元素执行不同的操作
        if (v[0]->asSymbol() == "define"s) {
            // 如果是 define 操作符，则进行定义操作
            if (auto name = v[1]->asSymbol()) {
                // 获取定义名称
                symbolList[std::dynamic_pointer_cast<SymbolValue>(v[1])->getName()] = eval(v[2]);
                // 返回空表表示成功
                return std::make_shared<NilValue>();
            }
            else {
                // define 操作符形式错误，抛出异常
                throw LispError("Malformed define.");
            }
        }
        else {
            auto proc = eval(v[0]);
            std::vector<ValuePtr> args = evalList(expr);
            return apply(proc, args);
        }
    }
    else if (expr->isSymbol()) {
        // 如果表达式是符号类型，则查找并返回符号对应的值

        auto name = std::dynamic_pointer_cast<SymbolValue>(expr)->getName();
        auto it = symbolList.find(name);
        if(it == symbolList.end())
            throw LispError("Variable " + name + " not defined.");
        else
            return ValuePtr(it->second);
    }
    else {
        // 其他类型的表达式暂未实现，抛出异常
        throw LispError("Unimplemented");
    }
}


std::vector<ValuePtr>EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::vector<ValuePtr> vec = PairParser(expr).parse();
    for(int i = 1; i < vec.size() - 1; i++) {
        result.push_back(eval(vec[i]));
    }
    return result;
}

ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr>& args) {
   if (proc->getType() == ValueType::BUILTIN_PROC_VALUE) {
       return std::dynamic_pointer_cast<BuiltinProcValue>(proc)->getFunction()(args);
   }
   else {
       throw LispError("Unimplemented");
   }
}
