#include "../include/value.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <typeinfo>

bool Value::isNil() const {
    if(type == ValueType::NIL_VALUE)
        return true;
    return false;
}

bool Value::isSelfEvaluating() const {
    if(type == ValueType::BOOLEAN_VALUE ||
       type == ValueType::NUMERIC_VALUE ||
       type == ValueType::STRING_VALUE)
        return true;
    return false;
}

bool Value::isList() const {
    if(type == ValueType::PAIR_VALUE)
        return true;
    return false;
}

bool Value::isSymbol() const {
    if(type == ValueType::SYMBOL_VALUE)
        return true;
    return false;
}

std::optional<std::string> Value::asSymbol() const {
   return std::nullopt;
}

std::optional<std::string> SymbolValue::asSymbol() const {
    return { name };
}

std::string BooleanValue::toString() const {
    return value? "#t" : "#f";
}

std::string NumericValue::toString() const {
    if(int(value) == value)
        return std::to_string(int(value));
    return std::to_string(value);
}

std::string StringValue::toString() const {
    std::stringstream ss;
    ss << std::quoted(value);
    return ss.str();
}

std::string NilValue::toString() const {
    return "()";
}

std::string SymbolValue::toString() const {
    return name;
}

const Value& pharase(std::vector<std::string>& result, const Value& ref) {
    if(typeid(ref) == typeid(PairValue&)) {
        const PairValue& r = dynamic_cast<const PairValue&>(ref);
        pharase(result, *(r.getLeft()));
        const Value& reff = pharase(result, *(r.getRight()));
        return reff;
    }
    else {
        result.push_back(ref.toString());
    }
    return ref;
}

std::string PairValue::toString() const {
    std::vector<std::string> vec{};
    const Value& ref = dynamic_cast<const Value&>(*this);
    const Value& v = pharase(vec, ref);
    std::string result = "(";
    if(typeid(v) == typeid(NilValue&)) {
        for(int i = 0; i < vec.size() - 1; i++) {
            result += (vec[i] + " ");
        }
        result.back() = ')';
    }
    else {
        for(int i = 0; i < vec.size() - 1; i++) {
            result += vec[i] + " ";
        }
        result += ". ";
        result += vec[vec.size() - 1] + ")";
    }
    return result;
}

void backtracking(std::vector<ValuePtr>& result, ValuePtr ptr) {
    if(ptr->isList()) {
        auto p = std::dynamic_pointer_cast<PairValue>(ptr);
        backtracking(result, p->getLeft());
        backtracking(result, p->getRight());
    }
    else {
        result.push_back(ptr);
    }
}

std::vector<ValuePtr> PairValue::toVector() const {
    std::vector<ValuePtr> vec{};
    ValuePtr ptr(const_cast<PairValue*>(this));
    backtracking(vec, ptr);
    return vec;
}
