#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <vector>
#include <optional>
#include <memory>


enum class ValueType {
    BOOLEAN_VALUE,
    NUMERIC_VALUE,
    STRING_VALUE,
    NIL_VALUE,
    SYMBOL_VALUE,
    PAIR_VALUE
};

class Value;

using ValuePtr = std::shared_ptr<Value>;

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

    virtual std::string toString() const = 0;

    bool isNil() const;

    bool isSelfEvaluating() const;

    bool isList() const;

    bool isSymbol() const;

    virtual std::optional<std::string> asSymbol() const;
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

    virtual std::string toString() const override;
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

    virtual std::string toString() const override;
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

    virtual std::string toString() const override;
};

class NilValue : public Value {
public:
    NilValue() : Value{ValueType::NIL_VALUE} {}

    NilValue(const NilValue& n) : Value(n) {}

    virtual ~NilValue() = default;

    virtual std::string toString() const override;
};

class SymbolValue : public Value {
private:
    std::string name;

public:
    SymbolValue() = delete;

    SymbolValue(const std::string& name) : Value{ValueType::SYMBOL_VALUE}, name{name} {}



    virtual ~SymbolValue() = default;

    const std::string& getName() const {
        return name;
    }

    virtual std::string toString() const override;

    virtual std::optional<std::string> asSymbol() const override;
};

class PairValue : public Value {
private:
    std::shared_ptr<Value> left;
    std::shared_ptr<Value> right;

public:
    PairValue() = delete;

    PairValue(const ValuePtr& left, const ValuePtr& right) : Value{ValueType::PAIR_VALUE}, left{left}, right{right} {}

    PairValue(const PairValue& p) : Value{p}, left{p.left}, right{p.right} {}

    virtual ~PairValue() = default;

    const auto getValue() const {
        return std::make_pair(left, right);
    }

    auto& getLeft() const {
        return left;
    }

    auto& getRight() const {
        return right;
    }

    virtual std::string toString() const override;

    std::vector<ValuePtr> toVector() const;
};

#endif  // VALUE_H
