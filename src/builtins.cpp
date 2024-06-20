/* ************************************************************************
> File Name:     builtins.cpp
> Author:        林毅波
> Created Time:  2024/5/23 21:26:41
> Description:   
 ************************************************************************/

#include "builtins.h"
#include "eval_env.h"
#include "reader.h"
#include <iostream>
#include <cmath>
#include <iterator>
#include "params_checker.h"

/**
 * @brief 实现加法操作，将多个数值相加并返回结果。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 加法结果
 * @throws LispError 如果参数包含非数值
 */
ValuePtr add(const std::vector<ValuePtr>& params, EvalEnv&) {
    double result = 0;
    for (const auto& i :params) {
        if (i->isNumber()) {
            result += *i->asNumber();
        }
        else {
            throw LispError("Cannot add a non-numeric value.");
        }
    }
    return std::make_shared<NumericValue>(result);
}

/**
 * @brief 实现打印操作，将参数的字符串表示输出到标准输出。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 空值
 */
ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv&) {
    for (const auto& param : params) {
        std::cout << param->toString();
    }
    return std::make_shared<NilValue>();
}

/**
 * @brief 实现乘法操作，将多个数值相乘并返回结果。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 乘法结果
 * @throws LispError 如果参数包含非数值
 */
ValuePtr multiply(const std::vector<ValuePtr>& params, EvalEnv&) {
    double result = 1;
    for (const auto& i : params) {
        if (i->isNumber()) {
            result *= *i->asNumber();
        }
        else {
            throw LispError("Cannot multiply a non-numeric value.");
        }
    }
    return std::make_shared<NumericValue>(result);
}

/**
 * @brief 实现减法操作，将多个数值依次相减并返回结果。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 减法结果
 * @throws LispError 如果参数包含非数值
 */
ValuePtr reduce(const std::vector<ValuePtr>& params, EvalEnv&) {
    std::vector<int> _range = {1, 2};
    auto checker = std::make_unique<ParamsChecker>(params, "-", 
        std::move(std::make_unique<RangeParamsNumberChecker>(_range)));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    if (params.size() == 1) {
        return std::make_shared<NumericValue>(-*params[0]->asNumber());
    }
    else {
        return std::make_shared<NumericValue>(*params[0]->asNumber() - *params[1]->asNumber());
    }
}

/**
 * @brief 实现apply操作，将参数应用于过程并返回结果。
 * 
 * @param params 参数列表
 * @param env 求值环境
 * @return 过程的执行结果
 * @throws LispError 如果参数不符合预期
 */
ValuePtr apply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto checker = std::make_unique<ParamsChecker>(params, "apply",
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->check();
    if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
        if (params[1]->isList() || params[1]->isNil()) {
            auto vec = params[1]->toVector();
            vec.pop_back();
            return std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()(vec, env);
        }
        else {
            throw LispError("expected a list that can be applied on a procedure");
        }
    }
    else if (params[0]->getType() == ValueType::LAMBDA_VALUE) {
        if (params[1]->isList()) {
            auto vec = params[1]->toVector();
            vec.pop_back();
            return std::dynamic_pointer_cast<LambdaValue>(params[0])->apply(vec);
        }
        else {
            throw LispError("exprceted a list that can be applied on a procecure");
        }
    }
    else {
        throw LispError("expected a procedure that can be applied to arguments");
    }
}

/**
 * @brief 实现display操作，将参数的字符串表示输出到标准输出。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 空值
 */
ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv&) {
    std::unique_ptr<ParamsChecker> checker = std::make_unique<ParamsChecker>(
        params, "display", std::move(std::make_unique<EqualParamsNumberChecker>(1)));
    checker->check();
    auto param = params[0];
    if (param->getType() == ValueType::STRING_VALUE) {
        std::cout << std::dynamic_pointer_cast<StringValue>(params[0])->getValue();
    }
    else {
        std::cout << "\'" << param->toString();
    }
    return std::make_shared<NilValue>();
}

/**
 * @brief 实现displayln操作，将参数的字符串表示输出到标准输出并换行。
 * 
 * @param params 参数列表
 * @param env 求值环境
 * @return 空值
 */
ValuePtr displayln(const std::vector<ValuePtr>& params, EvalEnv& env) {
    display(params, env);
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

/**
 * @brief 实现`error`操作，根据参数抛出LispError异常。
 * 
 * @param params 参数列表
 * @param env 求值环境
 * @return 无返回值
 * @throws LispError 如果参数不符合预期
 */
ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 1) {
        throw LispError("The error procedure need at most procedure, given" + std::to_string(static_cast<int>(params.size())));
    }
    if (params.size() == 0) {
        throw LispError("0");
    }
    throw LispError(params[0]->toString());
}

/**
 * @brief 实现`eval`操作，对参数进行求值并返回结果。
 * 
 * @param params 参数列表
 * @param env 求值环境
 * @return 参数的求值结果
 * @throws LispError 如果参数不符合预期
 */
ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto checker = std::make_unique<ParamsChecker>(params, "eval", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    return env.eval(params[0]);
}

/**
 * @brief 实现`_exit`操作，根据参数退出程序。
 * 
 * @param params 参数列表
 * @param env 求值环境
 * @return 无返回值
 * @throws LispError 如果参数不是数值
 */
ValuePtr _exit(const std::vector<ValuePtr>& params, EvalEnv& env) {
    std::vector<int> _range = {0, 1};
    auto checker = std::make_unique<ParamsChecker>(params, "exit", 
        std::make_unique<RangeParamsNumberChecker>(_range));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->check();
    if (params.size() == 0) {
        std::exit(0);
    }
    else {
        std::exit(*params[0]->asNumber());
    }
}

/**
 * @brief 实现`newline`操作，输出换行符。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 空值
 * @throws LispError 如果参数不为空
 */
ValuePtr newline(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.empty()) {
        std::cout << std::endl;
        return std::make_shared<NilValue>();
    }
    else {
        throw LispError("The procedure should not have params.");
    }
}

/**
 * @brief 实现`atom`操作，判断参数是否为原子值。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是原子值，返回true；否则返回false
 * @throws LispError 如果参数不符合预期
 */
ValuePtr atom(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "atom?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    if (params[0]->getType() == ValueType::BOOLEAN_VALUE ||
        params[0]->getType() == ValueType::NUMERIC_VALUE ||
        params[0]->getType() == ValueType::STRING_VALUE ||
        params[0]->getType() == ValueType::SYMBOL_VALUE ||
        params[0]->getType() == ValueType::NIL_VALUE) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

/**
 * @brief 实现`boolean`操作，判断参数是否为布尔值。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是布尔值，返回true；否则返回false
 * @throws LispError 如果参数不符合预期
 */
ValuePtr boolean(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "boolean?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    if (params[0]->getType() == ValueType::BOOLEAN_VALUE) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

/**
 * @brief 实现`integer`操作，判断参数是否为整数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是整数，返回true；否则返回false
 * @throws LispError 如果参数不符合预期
 */
ValuePtr integer(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "integer?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    if (params[0]->isNumber() && static_cast<int>(*params[0]->asNumber()) == *params[0]->asNumber()) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

/**
 * @brief 实现`list`操作，判断参数是否为列表。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是列表或空表，返回true；否则返回false
 * @throws LispError 如果参数不符合预期
 */
ValuePtr list(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "list?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    if (params[0]->isList() || params[0]->isNil()) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

/**
 * @brief 实现`number`操作，判断参数是否为数值。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是数值，返回true；否则返回false
 * @throws LispError 如果参数不符合预期
 */
ValuePtr number(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "number?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    if (params[0]->isNumber()) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

/**
 * @brief 实现`null`操作，判断参数是否为空表。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是空表，返回true；否则返回false
 * @throws LispError 如果参数不符合预期
 */
ValuePtr null(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "null?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    if (params[0]->isNil()) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

/**
 * @brief 实现`pair`操作，判断参数是否为非空列表。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是非空列表，返回true；否则返回false
 * @throws LispError 如果参数不符合预期
 */
ValuePtr _pair(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "pair?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    if (params[0]->getType() == ValueType::PAIR_VALUE) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

/**
 * @brief 实现`isProcedure`操作，判断参数是否为过程（内置过程或Lambda函数）。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是过程，返回true；否则返回false
 * @throws LispError 如果参数不符合预期
 */
ValuePtr isProcedure(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "procedure?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE || params[0]->getType() == ValueType::LAMBDA_VALUE) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

/**
 * @brief 实现`isString`操作，判断参数是否为字符串。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是字符串，返回true；否则返回false
 * @throws LispError 如果参数不符合预期
 */
ValuePtr isString(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "string?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    if (params[0]->getType() == ValueType::STRING_VALUE) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

/**
 * @brief 实现`isSymbol`操作，判断参数是否为符号。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是符号，返回true；否则返回false
 * @throws LispError 如果参数不符合预期
 */
ValuePtr isSymbol(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "symbol?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    if (params[0]->isSymbol()) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

namespace{
/**
 * @brief 实现`makeList`操作，根据参数向量构建一个列表。
 * 
 * @param vec 参数向量
 * @param start 起始索引
 * @return 构建的列表
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
 * @brief 实现`append`操作，将多个列表连接成一个新的列表。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 连接后的列表
 * @throws LispError 如果参数不符合预期
 */
ValuePtr append(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        return std::make_shared<NilValue>();
    }
    else if (params.size() == 1) {
        return params[0];
    }
    else {
        std::vector<ValuePtr> result{};
        for (int i = 0; i < static_cast<int>(params.size()) - 1; i++) {
            if (params[i]->isList()) {
                auto vec = params[i]->toVector();
                if (vec.back()->isNil()) 
                    vec.pop_back();
                std::ranges::copy(vec, std::back_inserter(result));
            }
            else if (!params[i]->isNil()) {
                throw LispError("The append procedure need a list param.");
            }
        }
        if (params.back()->isNil()) {
            result.push_back(params.back());
        }
        std::ranges::copy(params.back()->toVector(), std::back_inserter(result));
        return makeList(result, 0);
    }
}

/**
 * @brief 实现`car`操作，获取列表的第一个元素。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 列表的第一个元素
 * @throws LispError 如果参数不符合预期
 */
ValuePtr car(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "car", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->addTypeRequire(0, ValueType::PAIR_VALUE);
    checker->check();
    return std::dynamic_pointer_cast<PairValue>(params[0])->getLeft();
}

/**
 * @brief 实现`cdr`操作，获取列表的剩余部分（除第一个元素）。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 列表的剩余部分
 * @throws LispError 如果参数不符合预期
 */
ValuePtr cdr(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "cdr", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->addTypeRequire(0, ValueType::PAIR_VALUE);
    checker->check();
    return std::dynamic_pointer_cast<PairValue>(params[0])->getRight();
}

/**
 * @brief 实现`cons`操作，将两个参数构建成一个新的列表。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 构建的列表
 * @throws LispError 如果参数不符合预期
 */
ValuePtr cons(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "cons", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->check();
    return std::make_shared<PairValue>(params[0], params[1]);
}

/**
 * @brief 实现`length`操作，计算列表的长度。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 列表的长度
 * @throws LispError 如果参数不符合预期
 */
ValuePtr length(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "length", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    if (params[0]->isList() || params[0]->isNil()) {
        if (params[0]->isNil()) {
            return std::make_shared<NumericValue>(0);
        }
        else {
            return std::make_shared<NumericValue>(static_cast<int>(params[0]->toVector().size()) - 1);
        }
    }
    else {
        throw LispError("The length procedure need list param.");
    }
}

/**
 * @brief 实现`_list`操作，将参数构建成一个新的列表。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 构建的列表
 */
ValuePtr _list(const std::vector<ValuePtr>& params, EvalEnv&) {
    std::vector<ValuePtr> new_params{params};
    new_params.push_back(std::make_shared<NilValue>());
    return makeList(new_params, 0);
}

/**
 * @brief 实现`_map`操作，将过程应用于列表的每个元素。
 * 
 * @param params 参数列表
 * @param env 求值环境
 * @return 应用过程后的列表
 * @throws LispError 如果参数不符合预期
 */
ValuePtr _map(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto checker = std::make_unique<ParamsChecker>(params, "map", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->check();
    if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
        std::vector<ValuePtr>result{};
        if (params[1]->isList()) {
            auto vec = params[1]->toVector();
            vec.pop_back();
            std::ranges::transform(vec, std::back_inserter(result), [&env, &params](ValuePtr& ptr) -> ValuePtr {
                return std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()({ptr}, env);
            });
            result.push_back(std::make_shared<NilValue>());
            return makeList(result, 0);
        }
        else if (params[1]->isNil()) {
            return std::make_shared<NilValue>();
        }
        else {
            throw LispError("The map procedure need list param as the second param.");
        }
    }
    else if (params[0]->getType() == ValueType::LAMBDA_VALUE) {
        std::vector<ValuePtr> result{};
        if (params[1]->isList()) {
            auto vec = params[1]->toVector();
            vec.pop_back();
            std::ranges::transform(vec, std::back_inserter(result), [&params](ValuePtr& ptr) -> ValuePtr {
                return std::dynamic_pointer_cast<LambdaValue>(params[0])->apply({ptr});
            }) ;
            result.push_back(std::make_shared<NilValue>());
            return makeList(result, 0);
        }
        else if (params[1]->isNil()) {
            return std::make_shared<NilValue>();
        }
        else {
            throw LispError("The map procedure need list param as the second param.");
        }
    }
    else {
        throw LispError("The map procedure need procedure param as the first param.");
    }
}

namespace{
/**
 * @brief 将值转换为布尔类型。
 * 
 * @param ptr 要转换的值
 * @return 如果值为真，返回 true；否则返回 false
 */
bool change_to_bool(const ValuePtr& ptr) {
    if (ptr->getType() == ValueType::BOOLEAN_VALUE && !std::dynamic_pointer_cast<BooleanValue>(ptr)->getValue()) {
        return false;
    }
    else {
        return true;
    }
}
}

/**
 * @brief 实现 filter 过滤函数，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 筛选后的列表
 */
ValuePtr filter(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto checker = std::make_unique<ParamsChecker>(params, "filter", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->check();
    if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
        if (params[1]->isList()) {
            std::vector<ValuePtr> result{};
            auto vec = params[1]->toVector();
            vec.pop_back();
            std::ranges::copy_if(vec, std::back_inserter(result), [&env, &params](ValuePtr& ptr) -> bool {
                return change_to_bool(std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()({ptr}, env));
            });
            result.push_back(std::make_shared<NilValue>());
            return makeList(result, 0);
        }
        else if (params[1]->isNil()) {
            return std::make_shared<NilValue>();
        }
        else {
            throw LispError("The filter procedure need list param as the second param.");
        }
    }
    else if (params[0]->getType() == ValueType::LAMBDA_VALUE) {
        if (params[1]->isList()) {
            std::vector<ValuePtr> result{};
            auto vec = params[1]->toVector();
            vec.pop_back();
            std::ranges::copy_if(vec, std::back_inserter(result), [&params](ValuePtr& ptr) -> bool {
                return change_to_bool(std::dynamic_pointer_cast<LambdaValue>(params[0])->apply({ptr}));
            });
            result.push_back(std::make_shared<NilValue>());
            return makeList(result, 0);
        }
        else if (params[1]->isNil()) {
            return std::make_shared<NilValue>();
        }
        else {
            throw LispError("The filter procedure need list param as the second param.");
        }
    }
    else {
        throw LispError("The filter proceduren need procedure param as the first param.");
    }
}

/**
 * @brief 实现 reduce 函数，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境
 * @return 筛选后的结果
 */
ValuePtr _reduce(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto checker = std::make_unique<ParamsChecker>(params, "reduce", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->check();
    if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
        if (params[1]->isNil()) {
            throw LispError("The reduce procedure cannot receive a null list as the second param.");
        }
        else if (!params[1]->isList()) {
            throw LispError("The reduce procedure need a list param as the second param.");
        }
        if (std::dynamic_pointer_cast<NumericValue>(length({params[1]}, env))->getValue() == 1) {
            return car({params[1]}, env);
        }
        else {
            return std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()({car({params[1]}, env), _reduce({params[0], cdr({params[1]}, env)}, env)}, env);
        }
    }
    else if (params[0]->getType() == ValueType::LAMBDA_VALUE) {
        if (params[1]->isNil()) {
            throw LispError("The reduce procedure cannot receive a null list as the second param.");
        }
        else if (!params[1]->isList()) {
            throw LispError("The reduce procedure need a list param as the second param.");
        }
        if (std::dynamic_pointer_cast<NumericValue>(length({params[1]}, env))->getValue() == 1) {
            return car({params[1]}, env);
        }
        else {
            return std::dynamic_pointer_cast<LambdaValue>(params[0])->apply({car({params[1]}, env), _reduce({params[0], cdr({params[1]}, env)}, env)});
        }
    }
    else {
        throw LispError("The reduce procedure need a procedure param as the first param.");
    }
}

/**
 * @brief 实现除法函数，接受一个或两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 计算结果
 */
ValuePtr divide(const std::vector<ValuePtr>& params, EvalEnv&) {
    std::vector<int> _range = {1, 2};
    auto checker = std::make_unique<ParamsChecker>(params, "/", 
        std::make_unique<RangeParamsNumberChecker>(_range));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    if (params.size() == 1) {
        if (*params[0]->asNumber() == 0) {
            throw LispError("Division by zero.");
        }
        return std::make_shared<NumericValue>(1.0 / *params[0]->asNumber());
    }
    else {
        if (*params[1]->asNumber() == 0) {
            throw LispError("Division by zero.");
        }
        return std::make_shared<NumericValue>(*params[0]->asNumber() / *params[1]->asNumber());
    }
}

/**
 * @brief 实现绝对值函数，接受一个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 绝对值
 */
ValuePtr abs(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "abs", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->check();
    return std::make_shared<NumericValue>(std::abs(*params[0]->asNumber()));
}

/**
 * @brief 实现指数运算函数，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 计算结果
 * @throw LispError 如果底数和指数都为零，则抛出异常
 */
ValuePtr expt(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "expt", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    if (*params[0]->asNumber() == 0 && *params[1]->asNumber() == 0) {
        throw LispError("Undefined! The base and the exponent cannot be zero in the same time.");
    }
    return std::make_shared<NumericValue>(std::pow(*params[0]->asNumber(), *params[1]->asNumber()));
}

/**
 * @brief 实现整数除法函数，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 计算结果
 * @throw LispError 如果除数为零，则抛出异常
 */
ValuePtr quotient(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "quotient", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    double x = *params[0]->asNumber();
    double y = *params[1]->asNumber();
    if (y == 0) {
        throw LispError("Divide by zero.");
    }
    double result = x / y;
    if (static_cast<int>(result) == result) 
        return std::make_shared<NumericValue>(result);
    else 
        return std::make_shared<NumericValue>(static_cast<int>(result));
}


/**
 * @brief 实现取模运算函数，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 计算结果
 * @throw LispError 如果参数不是整数，则抛出异常
 */
ValuePtr modulo(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "2", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    double x = *params[0]->asNumber();
    double y = *params[1]->asNumber();
    if (static_cast<int>(x) != x || static_cast<int>(y) != y) {
        throw LispError("The modulo procedure should receive integers, given not-integers.");
    }
    if (y == 0) {
        throw LispError("Divide by zero.");
    }
    int a = static_cast<int>(x);
    int b = static_cast<int>(y);
    int result = a % b;
    if (result * b < 0) 
        result += b;
    return std::make_shared<NumericValue>(result);
}

/**
 * @brief 实现取余运算函数，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 计算结果
 * @throw LispError 如果参数不是整数，则抛出异常
 */
ValuePtr remainder(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "remainder", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    double x = *params[0]->asNumber();
    double y = *params[1]->asNumber();
    if (static_cast<int>(x) != x || static_cast<int>(y) != y) {
        throw LispError("The remainder procedure should receive integers, given not-integers.");
    }
    if (y == 0) {
        throw LispError("Divide by zero.");
    }
    int result = x - y * static_cast<int>(x / y);
    return std::make_shared<NumericValue>(result);
}

namespace{
/**
 * @brief 判断两个值是否相等。
 * 
 * @param p1 第一个值
 * @param p2 第二个值
 * @param env 求值环境
 * @return 如果两个值相等，返回 true；否则返回 false
 * @throw LispError 如果存在未实现的类型，则抛出异常
 */
bool isEqual(ValuePtr p1, ValuePtr p2, EvalEnv& env) {
    if (p1->getType() != p2->getType()) {
        return false;
    }
    else {
        if (p1->getType() == ValueType::BOOLEAN_VALUE) {
            return std::dynamic_pointer_cast<BooleanValue>(p1)->getValue() == std::dynamic_pointer_cast<BooleanValue>(p2)->getValue();
        }
        else if (p1->getType() == ValueType::NUMERIC_VALUE) {
            return std::dynamic_pointer_cast<NumericValue>(p1)->getValue() == std::dynamic_pointer_cast<NumericValue>(p2)->getValue();
        }
        else if (p1->getType() == ValueType::STRING_VALUE) {
            return std::dynamic_pointer_cast<StringValue>(p1)->getValue() == std::dynamic_pointer_cast<StringValue>(p2)->getValue();
        }
        else if (p1->getType() == ValueType::NIL_VALUE) {
            return true;
        }
        else if (p1->getType() == ValueType::SYMBOL_VALUE) {
            return std::dynamic_pointer_cast<SymbolValue>(p1)->getName() == std::dynamic_pointer_cast<SymbolValue>(p2)->getName();
        }
        else if (p1->getType() == ValueType::PAIR_VALUE) {
            return isEqual(car({p1}, env), car({p2}, env), env) && isEqual(cdr({p1}, env), cdr({p2}, env), env);
        }
        else if (p1->getType() == ValueType::BUILTIN_PROC_VALUE) {
            return std::dynamic_pointer_cast<BuiltinProcValue>(p1)->getFunction() == std::dynamic_pointer_cast<BuiltinProcValue>(p2)->getFunction();
        }
        else if (p1->getType() == ValueType::LAMBDA_VALUE) {
            return p1.get() == p2.get();
        }
        else {
            throw LispError("Unimplement in equal?.");
        }
    }
}
}

/**
 * @brief 判断两个值是否相等（eq?）。
 * 
 * @param params 参数列表
 * @param env 求值环境
 * @return 如果两个值相等，返回 true；否则返回 false
 */
ValuePtr _eq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto checker = std::make_unique<ParamsChecker>(params, "eq?", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->check();
    if (params[0]->isSelfEvaluating() && params[1]->isSelfEvaluating()) {
        return std::make_shared<BooleanValue>(isEqual(params[0], params[1], env));
    }
    else if (params[0]->isSymbol() && params[1]->isSymbol()) {
        return std::make_shared<BooleanValue>(isEqual(params[0], params[1], env));
    }
    else if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE && params[1]->getType() == ValueType::BUILTIN_PROC_VALUE) {
        return std::make_shared<BooleanValue>(isEqual(params[0], params[1], env));
    }
    else if (params[0]->getType() == ValueType::NIL_VALUE && params[1]->getType() == ValueType::NIL_VALUE) {
        return std::make_shared<BooleanValue>(true);
    }
    if (params[0].get() == params[1].get()) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

/**
 * @brief 判断两个值是否相等（equal?）。
 * 
 * @param params 参数列表
 * @param env 求值环境
 * @return 如果两个值相等，返回 true；否则返回 false
 */
ValuePtr _equal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto checker = std::make_unique<ParamsChecker>(params, "equal?", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->check();
    return std::make_shared<BooleanValue>(isEqual(params[0], params[1], env));
}

/**
 * @brief 实现逻辑非操作，接受一个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 非操作的结果
 */
ValuePtr _not(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "not", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->check();
    return std::make_shared<BooleanValue>(!change_to_bool(params[0]));
}

/**
 * @brief 实现数值相等判断，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果两个数值相等，返回 true；否则返回 false
 */
ValuePtr __equal(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "=", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    return std::make_shared<BooleanValue>(*params[0]->asNumber() == *params[1]->asNumber());
}

/**
 * @brief 实现小于判断，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果第一个数值小于第二个数值，返回 true；否则返回 false
 */
ValuePtr less(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "<", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    return std::make_shared<BooleanValue>(*params[0]->asNumber() < *params[1]->asNumber());
}

/**
 * @brief 实现大于判断，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果第一个数值大于第二个数值，返回 true；否则返回 false
 */
ValuePtr larger(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, ">", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    return std::make_shared<BooleanValue>(*params[0]->asNumber() > *params[1]->asNumber());
}

/**
 * @brief 实现小于等于判断，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果第一个数值小于等于第二个数值，返回 true；否则返回 false
 */
ValuePtr less_or_equal(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "<=", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    return std::make_shared<BooleanValue>(*params[0]->asNumber() <= *params[1]->asNumber());
}

/**
 * @brief 实现大于等于判断，接受两个参数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果第一个数值大于等于第二个数值，返回 true；否则返回 false
 */
ValuePtr larger_or_equal(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, ">=", 
        std::make_unique<EqualParamsNumberChecker>(2));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->addTypeRequire(1, ValueType::NUMERIC_VALUE);
    checker->check();
    return std::make_shared<BooleanValue>(*params[0]->asNumber() >= *params[1]->asNumber());
}

/**
 * @brief 判断一个整数是否为偶数。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数是偶数，返回 true；否则返回 false
 * @throw LispError 如果参数不是整数，则抛出异常
 */
ValuePtr even(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "even?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->check();
    if (static_cast<int>(*params[0]->asNumber()) != *params[0]->asNumber()) {
        throw LispError("The procedure even? cannot receive a non-integer value.");
    }
    return std::make_shared<BooleanValue>(static_cast<int>(*params[0]->asNumber()) % 2 == 0);
}

/**
 * @brief 判断一个整数是否为奇数。
 * 
 * @param params 参数列表
 * @param env 求值环境
 * @return 如果参数是奇数，返回 true；否则返回 false
 * @throw LispError 如果参数不是整数，则抛出异常
 */
ValuePtr odd(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto checker = std::make_unique<ParamsChecker>(params, "odd?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->check();
    if (static_cast<int>(*params[0]->asNumber()) != *params[0]->asNumber()) {
        throw LispError("The procedure odd? cannot receive a non-integer value.");
    }
    return std::make_shared<BooleanValue>(static_cast<int>(*params[0]->asNumber()) % 2 != 0);
}

/**
 * @brief 判断一个数值是否为零。
 * 
 * @param params 参数列表
 * @param env 求值环境（未使用）
 * @return 如果参数等于零，返回 true；否则返回 false
 */
ValuePtr zero(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "zero?", 
        std::make_unique<EqualParamsNumberChecker>(1));
    checker->addTypeRequire(0, ValueType::NUMERIC_VALUE);
    checker->check();
    return std::make_shared<BooleanValue>(*params[0]->asNumber() == 0);
}

ValuePtr _read(const std::vector<ValuePtr>& params, EvalEnv&) {
    auto checker = std::make_unique<ParamsChecker>(params, "read", 
        std::make_unique<EqualParamsNumberChecker>(0));
    checker->check();
    if (Reader::getInstance().empty()) {
        Reader::getInstance().readIn();
    }
    return Reader::getInstance().output();
}

std::unordered_map<std::string, BuiltinFuncType*> innerSymbolTable{
    {"+", &add},
    {"*", &multiply}, 
    {"-", &reduce}, 
    {"/", &divide}, 
    {"abs", &abs}, 
    {"expt", &expt}, 
    {"quotient", &quotient}, 
    {"modulo", &modulo}, 
    {"remainder", &remainder}, 
    {"eq?", &_eq}, 
    {"equal?", &_equal}, 
    {"not", &_not}, 
    {"=", &__equal}, 
    {"<", &less}, 
    {">", &larger}, 
    {"<=", &less_or_equal}, 
    {">=", &larger_or_equal}, 
    {"even?", &even}, 
    {"odd?", &odd}, 
    {"zero?", &zero}, 
    {"apply", &apply}, 
    {"display", &display}, 
    {"displayln", &displayln}, 
    {"error", &error}, 
    {"exit", &_exit}, 
    {"newline", &newline}, 
    {"print", &print}, 
    {"eval", &eval}, 
    {"atom?", &atom}, 
    {"boolean?", &boolean}, 
    {"integer?", &integer}, 
    {"list?", &list}, 
    {"number?", &number}, 
    {"null?", &null}, 
    {"pair?", &_pair}, 
    {"procedure?", &isProcedure}, 
    {"string?", &isString}, 
    {"symbol?", &isSymbol}, 
    {"append", &append}, 
    {"car", &car}, 
    {"cdr", &cdr}, 
    {"cons", &cons}, 
    {"length", &length}, 
    {"len", &length}, 
    {"list", &_list}, 
    {"map", &_map}, 
    {"filter", &filter}, 
    {"reduce", &_reduce}, 
    {"read", &_read}
};
