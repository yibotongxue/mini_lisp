#ifndef VALUE_H
#define VALUE_H

#include <string>

enum class ValueType {
    BOOLEAN_VALUE,
    NUMERIC_VALUE,
    STRING_VALUE,
    NIL_VALUE,
    SYMBOL_VALUE,
    PAIR_VALUE
};

class Value {
private:
    ValueType type;

protected:
    Value(ValueType type) : type{type} {}

    Value(const Value& v) : type{v.type} {}
public:
    virtual ~Value() = default;

    ValueType getType() const {
        return type;
    }
};

class BooleanValue : public Value {
private:
    bool value;

public:
    BooleanValue() = delete;

    BooleanValue(bool value) : Value{ValueType::BOOLEAN_VALUE}, value{value} {}

    BooleanValue(const BooleanValue& b) : Value{b}, value{b.value} {}

    virtual ~BooleanValue() = default;

    bool getValue() const {
        return value;
    }
};

class NumericValue : public Value {
private:
    double value;
public:
    NumericValue() = delete;

    NumericValue(double value) : Value{ValueType::NUMERIC_VALUE}, value{value} {}

    NumericValue(const NumericValue& n) : Value{n}, value{n.value} {}

    virtual ~NumericValue() = default;

    double getValue() const {
        return value;
    }
};

class StringValue : public Value {
private:
    std::string value;

public:
    StringValue() = delete;

    StringValue(const std::string& value) : Value{ValueType::STRING_VALUE}, value{value} {}

    StringValue(const StringValue& s) : Value{s}, value{s.value} {}

    virtual ~StringValue() = default;

    const std::string& getValue() const {
        return value;
    }
};

class NilValue : public Value {
public:
    NilValue() : Value{ValueType::NIL_VALUE} {}

    NilValue(const NilValue& n) : Value(n) {}

    virtual ~NilValue() = default;
};

class SymbolValue : public Value {
private:
    std::string name;

public:
    SymbolValue() = delete;

    SymbolValue(const std::string& name) : Value{ValueType::SYMBOL_VALUE}, name{name} {}

    SymbolValue(const SymbolValue& symbol) : Value{symbol}, name{symbol.name} {}

    virtual ~SymbolValue() = default;

    const std::string& getName() const {
        return name;
    }
};

#endif  // VALUE_H