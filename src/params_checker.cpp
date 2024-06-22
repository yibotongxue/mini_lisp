#include "params_checker.h"
#include <iostream>

namespace {
/**
 * @brief 将枚举类型 ValueType 转换为字符串
 * 
 * @param - type 类型
 * @return - 类型的字符串表示
 * @throw 当出现为定义的类型的时候
 */
std::string change_to_string(const ValueType& type) {
    if (type == ValueType::BOOLEAN_VALUE) {
        return "boolean";
    }
    else if (type == ValueType::BUILTIN_PROC_VALUE) {
        return "builtin procedure";
    }
    else if (type == ValueType::LAMBDA_VALUE) {
        return "lambda";
    }
    else if (type == ValueType::NIL_VALUE) {
        return "nil";
    }
    else if (type == ValueType::RATIONAL_VALUE) {
        return "rational";
    }
    else if (type == ValueType::NUMERIC_VALUE) {
        return "number";
    }
    else if (type == ValueType::PAIR_VALUE) {
        return "pair";
    }
    else if (type == ValueType::STRING_VALUE) {
        return "string";
    }
    else if (type == ValueType::SYMBOL_VALUE) {
        return "symbol";
    }
    else if (type == ValueType::RATIONAL_VALUE) {
        return "rational";
    }
    else {
        throw std::runtime_error("Unimplement");
    }
}
}

/**
 * @brief 检查参数数量是否正确。
 * 
 * @param params 参数列表
 * @param name 函数名称
 * @throws LispError 如果参数数量不匹配
 */

void EqualParamsNumberChecker::numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const {
    int _size = static_cast<int>(params.size());
    if (_size != n) {
        throw LispError("The procedure " + name + " need " + std::to_string(n) + " params, given " + std::to_string(_size) + ".");
    }
    assert(_size == n);
    return;
}

/**
 * @brief 检查参数数量是否满足最小要求。
 * 
 * @param params 参数列表
 * @param name 函数名称
 * @throws LispError 如果参数数量不足
 */

void LargerParamsNumberChecker::numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const {
    int _size = static_cast<int>(params.size());
    if (_size < n) {
        throw LispError("The procedure " + name + " need at least " + std::to_string(n) + " params, given " + std::to_string(_size) + ".");
    }
    assert(_size >= n);
    return;
}

/**
 * @brief 检查参数数量是否不超过最大限制。
 * 
 * @param params 参数列表
 * @param name 函数名称
 * @throws LispError 如果参数数量超过最大限制
 */

void LesserParamsNumberChecker::numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const {
    int _size = static_cast<int>(params.size());
    if (_size > n) {
        throw LispError("The procedure " + name + " need at most " + std::to_string(n) + " params, given " + std::to_string(_size) + ".");
    }
    assert(_size <= n);
    return;
}

/**
 * @brief 将存储的数字转换为字符串。
 * 
 * @return 转换后的字符串
 * @throws std::runtime_error 如果容器为空
 */
std::string RangeParamsNumberChecker::get_string() const {
    int _size = static_cast<int>(numbers.size());
    if (_size <= 0) {
        throw std::runtime_error("Unimplement.");
    }
    if (_size == 1) {
        return std::to_string(numbers[0]);
    }
    else if (_size == 2) {
        return std::to_string(numbers[0]) + " or " + std::to_string(numbers[1]);
    }
    else {
        std::string result;
        for (int i = 0; i < _size - 2; i++) {
            result += std::to_string(numbers[i]) + ", ";
        }
        result += std::to_string(numbers[_size - 2]) + " or " + std::to_string(numbers[_size - 1]);
        return result;
    }
}

/**
 * @brief 检查参数数量是否在预期范围内。
 * 
 * @param params 参数列表
 * @param name 函数名称
 * @throws LispError 如果参数数量超出指定范围
 */
void RangeParamsNumberChecker::numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const {
    std::string s = get_string();
    int _size = static_cast<int>(params.size());
    if (std::find(numbers.begin(), numbers.end(), _size) == numbers.end()) {
        throw LispError("The " + name + " procedure could only receive " + s + " params, given " + std::to_string(_size) + ".");
    }
    assert(std::find(numbers.begin(), numbers.end(), _size) != numbers.end());
    return;
}

/**
 * @brief 将存储的参数类型转换为字符串表示。
 * 
 * @return 参数类型的字符串表示
 * @throws std::runtime_error 如果类型列表为空
 */

std::string ParamTypeChecker::get_string() const {
    int _size = static_cast<int>(types.size());
    if (_size <= 0) {
        throw std::runtime_error("Unimplement.");
    }
    if (_size == 1) {
        return change_to_string(types[0]);
    }
    else if (_size == 2) {
        return change_to_string(types[0]) + " or " + change_to_string(types[1]);
    }
    else {
        std::string result;
        for (int i = 0; i < _size - 2; i++) {
            result += change_to_string(types[i]) + ", ";
        }
        result += change_to_string(types[_size - 2]) + " or " + change_to_string(types[_size - 1]);
        return result;
    }
}

/**
 * @brief 检查参数类型是否符合预期。
 * 
 * @param params 参数列表
 * @param name 函数名称
 * @throws LispError 如果参数类型不匹配
 */
void ParamTypeChecker::checkType(const std::vector<ValuePtr>& params, const std::string& name) {
    auto ptr = params[n];
    bool inTypes = false;
    for (auto& type : types) {
        if (ptr->getType() == type) {
            inTypes = true;
            break;
        }
    }
    if (!inTypes) {
        throw LispError("The " + std::to_string(n) + " param of the " + name + " procedure should be a " +
         get_string() + " type value, given a " + change_to_string(ptr->getType()) + " type.");
    }
    assert(inTypes);
    return;
}

/**
 * @brief 构造函数 ParamsChecker 的初始化列表。
 * 
 * @param params 参数列表
 * @param name 函数名称
 * @param numberChecker 参数数量检查器
 */
ParamsChecker::ParamsChecker(const std::vector<ValuePtr>& params, const std::string& name, std::unique_ptr<ParamsNumberChecker> numberChecker) 
            : params{params}, name{name}, numberChecker{std::move(numberChecker)}, typeCheckers{} {}

/**
 * @brief 向类型检查器列表中添加一个 ParamTypeChecker 对象。
 * 
 * @param n 参数索引
 * @param requiredTypes 所需的参数类型列表
 */
void ParamsChecker::addTypeRequire(int n, const std::vector<ValueType>& requiredTypes) {
    if (n >= 0 && n < params.size())
        typeCheckers.push_back(std::make_shared<ParamTypeChecker>(requiredTypes, n));
}

/**
 * @brief 检查参数的数量和类型是否符合预期。
 * 
 * @details 首先，调用参数数量检查器（`numberChecker`）来验证参数数量是否正确。
 *          然后，遍历类型检查器列表（`typeCheckers`），逐个检查参数类型是否匹配。
 * 
 * @throws LispError 如果参数数量或类型不匹配
 */

void ParamsChecker::check() {
    numberChecker->numberCheck(params, name);
    for (auto& typeRequire : typeCheckers) {
        typeRequire->checkType(params, name);
    }
}