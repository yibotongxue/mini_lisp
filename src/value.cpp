#include "../include/value.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <typeinfo>

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

const Value* pharase(std::vector<std::string>& result, const Value* ptr) {
    if(dynamic_cast<const PairValue*>(ptr)) {
        const PairValue* p = dynamic_cast<const PairValue*>(ptr);
        pharase(result, p->getLeft().get());
        return pharase(result, p->getRight().get());
    }
    else {
        result.push_back(ptr->toString());
    }
    return ptr;
}

std::string PairValue::toString() const {
    std::vector<std::string> vec{};
    const Value* ptr = dynamic_cast<const Value*>(this);
    const Value* v = pharase(vec, ptr);
    std::string result = "(";
    if(dynamic_cast<const NilValue*>(v)) {
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