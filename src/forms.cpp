/* ************************************************************************
> File Name:     forms.cpp
> Author:        林毅波
> Created Time:  2024/5/26 14:04:26
> Description:   
 ************************************************************************/

#include "../include/forms.h"
#include "../include/error.h"
#include "../include/builtins.h"
#include <iostream>

/**
 * @brief 定义函数，将变量或函数绑定到求值环境中。
 * 
 * @param args 参数列表，包含要定义的名称和对应的值
 * @param env 求值环境
 * @return 表示成功的NilValue对象
 * @throws LispError 如果定义不符合规范
 */
ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() <= 2) {
        throw LispError("Nothing found to define " + *args[1]->asSymbol());
    }
    if (args.size() == 3) {
        throw LispError("Nothing found to define " + *args[1]->asSymbol());
    }
    if (auto name = args[1]->asSymbol()) { // 定义变量
        auto first_Name = *args[1]->asSymbol();

        auto second_Value = args[2];

        env.defineBinding(first_Name, env.eval(args[2]));

        return std::make_shared<NilValue>();
    }
    else if (std::dynamic_pointer_cast<PairValue>(args[1])->isList()) { // 定义函数
        if (args[1]->isList()) {
            auto functionName = std::dynamic_pointer_cast<PairValue>(args[1])->getLeft();
            std::vector<std::string> params{};
            if (functionName->isSymbol()) { // 函数名字
                auto paramPtrs = args[1]->toVector();
                for (int i = 1; i < static_cast<int>(paramPtrs.size()) - 1; i++) {
                    if (paramPtrs[i]->isSymbol()){
                        params.push_back(*paramPtrs[i]->asSymbol());
                    }
                }
                std::vector<ValuePtr> functionBody{args.begin() + 2, args.end() - 1}; // 函数体
                env.defineBinding(*functionName->asSymbol(), std::make_shared<LambdaValue>(params, functionBody, env.shared_from_this()));
                return std::make_shared<NilValue>();
            }
            else {
                throw LispError("Malformed define.");
            }
        }
        else {
            throw LispError("Malformed define.");
        }
    }
    else {
        throw LispError("Malformed define.");
    }
}

/**
 * @brief 引用形式，获取引用的对象
 * @param args 参数列表，包含引用的对象
 * @param env 求值环境
 * @return 引用的对象
 * @throws LispError 如果参数数目不符合规范
 */
ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 3) // quote特殊形式、引用内容、空表
        throw LispError("Nothing to quoted.");
    else if (args.size() > 3) 
        throw LispError("The args are more than needed.");
    return args[1];
}

namespace{
/**
 * @brief 给定变量和求值环境，获取其布尔表示
 * 
 * @return 如果是虚值，也就是 #f，返回 false，否则返回 true
 */
bool change_to_bool(ValuePtr ptr, EvalEnv& env) {
    if (env.eval(ptr)->getType() == ValueType::BOOLEAN_VALUE && !std::dynamic_pointer_cast<BooleanValue>(env.eval(ptr))->getValue()) {
        return false;
    }
    else {
        return true;
    }
}
}

/**
 * @brief 实现if表达式，根据条件执行不同的分支。
 * 
 * @param args 参数列表，包含条件、真分支和假分支
 * @param env 求值环境
 * @return 根据条件执行的结果
 * @throws LispError 如果参数数量不符合规范
 */
ValuePtr ifForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    if (args.size() == 2) {
        throw LispError("No conditional statements are defined");
    }
    else if (args.size() == 3) {
        throw LispError("There is no true branch defined");
    }
    else if (args.size() > 5) {
        throw LispError("More sentence than expected.");
    }
    if (change_to_bool(args[1], env)) {
        return env.eval(args[2]);
    }
    else {
        if (args[3]->isNil())
            return args[3];
        return env.eval(args[3]);
    }
}

/**
 * @brief 实现and表达式，逐个检查条件，如果所有条件都为真，则返回true；否则返回false。
 * 
 * @param args 参数列表，包含多个条件
 * @param env 求值环境
 * @return 所有条件为真时返回true，否则返回false
 */
ValuePtr andForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    for (int i = 1; i < static_cast<int>(args.size()) - 1; i++) {
        if (!change_to_bool(args[i], env))
            return std::make_shared<BooleanValue>(false);
    }
    if (args.size() == 2)
        return std::make_shared<BooleanValue>(true);
    else
        return env.eval(args[args.size() - 2]);
}

/**
 * @brief 实现or表达式，逐个检查条件，如果有一个条件为真，则返回该条件的值；否则返回false。
 * 
 * @param args 参数列表，包含多个条件
 * @param env 求值环境
 * @return 有一个条件为真时返回该条件的值，否则返回false
 */
ValuePtr orForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    for (int i = 1; i < static_cast<int>(args.size()) - 1; i++) {
        if (change_to_bool(args[i], env))
            return env.eval(args[i]);
    }
    return std::make_shared<BooleanValue>(false);
}

/**
 * @brief 实现lambda表达式，创建一个匿名函数。
 * 
 * @param args 参数列表，包含参数列表和函数体
 * @param env 求值环境
 * @return Lambda函数对象
 * @throws LispError 如果参数数量不符合规范或参数不是符号
 */
ValuePtr lambdaForm(const std::vector<ValuePtr>&args, EvalEnv& env) {
    if (args.size() == 2) {
        throw LispError("The params are missing.");
    }
    else if (args.size() == 3) {
        throw LispError("The name of the process body is missing.");
    }
    else {
        auto paramsPtr = args[1]->toVector();
        std::vector<std::string> params{};
        for (int i = 0; i < static_cast<int>(paramsPtr.size()) - 1; i++) {
            if (paramsPtr[i]->isSymbol()) {
                params.push_back(*paramsPtr[i]->asSymbol());
            }
            else {
                throw LispError("Not symbol in params.");
            }
        }
        std::vector<ValuePtr> body{};
        for (int i = 2; i < static_cast<int>(args.size()) - 1; i++) {
            body.push_back(args[i]);
        }
        return std::make_shared<LambdaValue>(params, body, env.shared_from_this());
    }
}

/**
 * @brief 实现cond表达式，根据条件执行不同的分支。
 * 
 * @param args 参数列表，包含多个条件和对应的表达式
 * @param env 求值环境
 * @return 根据条件执行的结果
 * @throws LispError 如果参数数量不符合规范或条件不是符号或else不是在最后一个分支
 */
ValuePtr condForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() <= 2) { // cond 和 空表
        return std::make_shared<NilValue>();
    }
    else {
        ValuePtr* result = nullptr;
        for (int i = 1; i < static_cast<int>(args.size()) - 1; i++) {
            auto new_args = args[i]->toVector();
            if (new_args.size() <= 1) {
                throw LispError("There must be at least one sentence in cond.");
            }
            auto condition = new_args[0];
            std::vector<ValuePtr> results{};
            if (condition->isSymbol() && *condition->asSymbol() == "else") {
                if (i == args.size() - 2) {
                    for (int i = 1; i < static_cast<int>(new_args.size()) - 1; i++) {
                        results.push_back(env.eval(new_args[i])); // 逐个执行
                    }
                    if (results.empty()) {
                        return std::make_shared<NilValue>();
                    }
                    else {
                        return results.back();
                    }
                }
                else {
                    throw LispError("THe else is not in the last.");
                }
            }
            else {
            results.push_back(env.eval(new_args[0]));
            if (change_to_bool(condition, env)) {
                    for (int i = 1; i < static_cast<int>(new_args.size()) - 1; i++) {
                        results.push_back(env.eval(new_args[i])); // 逐个执行
                    }
                    return results.back();
                }
            }
        }
        return std::make_shared<NilValue>();
    }
}

/**
 * @brief 实现begin表达式，按顺序执行多个表达式，并返回最后一个表达式的结果。
 * 
 * @param args 参数列表，包含多个表达式
 * @param env 求值环境
 * @return 最后一个表达式的结果
 * @throws LispError 如果参数数量不符合规范
 */
ValuePtr beginForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2) {
        throw LispError("Error!");
    }
    for (int i = 1; i < static_cast<int>(args.size()) - 1; i++) {
        if (i == args.size() - 2) {
            return env.eval(args[i]);
        }
        else {
            env.eval(args[i]);
        }
    }
    return std::make_shared<NilValue>();
}

/**
 * @brief 实现let表达式，创建一个局部作用域，定义变量并执行函数体。
 * 
 * @param args 参数列表，包含变量定义和函数体
 * @param env 求值环境
 * @return 函数体的执行结果
 * @throws LispError 如果参数数量不符合规范或参数不是符号
 */
ValuePtr letForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() <= 2) {
        throw LispError("The let form need variable sentence.");
    }
    else if (args.size() == 3) {
        throw LispError("The let form need function body.");
    }
    else {
        std::vector<std::string> params{};
        std::vector<ValuePtr> new_args{};
            auto params_args = args[1]->toVector();
            for (int i = 0; i < static_cast<int>(params_args.size()) - 1; i++) {
                if (params_args[i]->isList()) {
                    auto vec = params_args[i]->toVector();
                    if (vec.size() <= 2) {
                        throw LispError("The param name or value is missing.");
                    }
                    else if (vec.size() >= 3) {
                        if (vec[0]->isSymbol()) {
                            params.push_back(*vec[0]->asSymbol());
                        }
                        else {
                            throw LispError("The param name should be a symbol type.");
                        }
                        new_args.push_back(env.eval(vec[1]));
                    }
                    else {
                        throw LispError("The param list need param name and value, given 3 object.");
                    }
                }
                else {
                    throw LispError("The let form here need a list.");
                }
            }
            std::vector<ValuePtr> body{args.begin() + 2, args.end() - 1};
            auto lambda = std::make_shared<LambdaValue>(params, body, env.shared_from_this());
            return lambda->apply(new_args);
    }
}

namespace{
/**
 * @brief 递归函数，用于构建一个列表（PairValue）。
 * 
 * @param vec 包含要构建列表的元素的向量
 * @param start 起始索引
 * @return 构建的列表对象
 */
ValuePtr makeList(std::vector<ValuePtr>& vec, int start) {
    if (vec.size() == 0) {
        return std::make_shared<NilValue>();
    }
    else if(vec.size() == 1) {
        return vec[0];
    }
    if (start == vec.size() - 2) {
        return std::make_shared<PairValue>(vec[start], vec[start + 1]);
    }
    else {
        return std::make_shared<PairValue>(vec[start], makeList(vec, start + 1));
    }
}
}

/**
 * @brief 实现quasiquote表达式，用于处理模板字符串。
 * 
 * @param args 参数列表，包含模板字符串和相关表达式
 * @param env 求值环境
 * @return 处理后的结果
 * @throws LispError 如果参数数量不符合规范或参数不是符号
 */
ValuePtr quasiquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() <= 2) {
        throw LispError("The quasiquote form need a arg.");
    }
    else if (args.size() == 3) {
        auto vec = args[1]->toVector();
        std::vector<ValuePtr> new_vec{};
        if (vec.size() == 0) {
            return std::make_shared<NilValue>();
        }
        else {
            for (int i = 0; i < static_cast<int>(vec.size()) - 1; i++) {
                    auto vecVec = vec[i]->toVector();
                    if (vecVec.size() == 3 && vecVec[0]->isSymbol() && *vecVec[0]->asSymbol() == "unquote") {
                        new_vec.push_back(env.eval(vecVec[1]));
                    }
                    else {
                        new_vec.push_back(vec[i]);
                    }
            }
            new_vec.push_back(std::make_shared<NilValue>());
            return makeList(new_vec, 0);
        }
    }
    else {
        throw LispError("The quasiquote form need 1 args.");
    }
}

// 特殊形式的无序映射
std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", defineForm},
    {"quote", quoteForm},
    {"if", ifForm},
    {"and", andForm},
    {"or", orForm}, 
    {"lambda", lambdaForm}, 
    {"cond", condForm}, 
    {"begin", beginForm}, 
    {"let", letForm}, 
    {"quasiquote", quasiquoteForm}
};
