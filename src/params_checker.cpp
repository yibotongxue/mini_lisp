#include "params_checker.h"
#include <iostream>

namespace {
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

void EqualParamsNumberChecker::numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const {
    int _size = static_cast<int>(params.size());
    if (_size != n) {
        throw LispError("The procedure " + name + " need " + std::to_string(n) + " params, given " + std::to_string(_size) + ".");
    }
    assert(_size == n);
    return;
}

void LargerParamsNumberChecker::numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const {
    int _size = static_cast<int>(params.size());
    if (_size < n) {
        throw LispError("The procedure " + name + " need at least " + std::to_string(n) + " params, given " + std::to_string(_size) + ".");
    }
    assert(_size >= n);
    return;
}

void LesserParamsNumberChecker::numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const {
    int _size = static_cast<int>(params.size());
    if (_size > n) {
        throw LispError("The procedure " + name + " need at most " + std::to_string(n) + " params, given " + std::to_string(_size) + ".");
    }
    assert(_size <= n);
    return;
}

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

void RangeParamsNumberChecker::numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const {
    std::string s = get_string();
    int _size = static_cast<int>(params.size());
    if (std::find(numbers.begin(), numbers.end(), _size) == numbers.end()) {
        throw LispError("The " + name + " procedure could only receive " + s + " params, given " + std::to_string(_size) + ".");
    }
    assert(std::find(numbers.begin(), numbers.end(), _size) != numbers.end());
    return;
}

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

ParamsChecker::ParamsChecker(const std::vector<ValuePtr>& params, const std::string& name, std::unique_ptr<ParamsNumberChecker> numberChecker) 
            : params{params}, name{name}, numberChecker{std::move(numberChecker)}, typeCheckers{} {}

void ParamsChecker::addTypeRequire(int n, const std::vector<ValueType>& requiredTypes) {
    if (n >= 0 && n < params.size())
        typeCheckers.push_back(std::make_shared<ParamTypeChecker>(requiredTypes, n));
}

void ParamsChecker::check() {
    numberChecker->numberCheck(params, name);
    for (auto& typeRequire : typeCheckers) {
        typeRequire->checkType(params, name);
    }
}