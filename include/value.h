#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <functional>


/**
 * @brief 表示Mini-Lisp的所有可能的数据类型
 * 
 * 此枚举类定义了所有可能的数据类型，包括数类型、布尔类型、字符串类型、符号类型、空表类型、对子类型
*/
enum class ValueType {
    BOOLEAN_VALUE,  // 布尔类型
    NUMERIC_VALUE,  // 数类型
    STRING_VALUE,   // 字符串类型
    NIL_VALUE,      // 空表类型
    SYMBOL_VALUE,   // 符号类型
    PAIR_VALUE,      // 对子类型
    BUILTIN_PROC_VALUE,                // 过程类型
    LAMBDA_VALUE
};

class Value;   // 值类

using ValuePtr = std::shared_ptr<Value>;  // Value 的智能指针

/* @brief Value类是一个抽象类，表示Mini-Lisp的数据类型。
 * 
 * 这个类作为 Mini-Lisp数据类型的基类，派生类表示的数据类型包括布尔类型、
 * 数类型、字符串类型、空表类型、符号类型、对子类型等。Value 类提供了一个
 * 用于管理数据类型的公共接口，定义了纯虚函数 toString 用于将对象转换为
 * 字符串以输出，派生类必须实现这个函数
 * 
 * Value类定义了值的数据类型，并定义了虚函数 asSymbol 用于处理对象作为符号类型
 * 的符号表，定义了判断函数来判断对象是否为空表、是否为自求值类型，是否为列表等。
 * 
 * @note 这个类用来作为具体数据类型的基类，派生类必须实现纯虚函数 toString()。
*/
class Value {
private:
    ValueType type; // 值的数据类型

protected:

    /**
     * @brief 转换构造函数，从枚举类型构造 Value 对象
     * 
     * @param type 值的数据类型
    */
    Value(ValueType type) : type{type} {}

    /**
     * @brief 从一个给定的值构造新的值对象
     * 
     * @param value 复制构造是拷贝的对象
    */
    Value(const Value& value) : type{value.type} {}

public:

    /**
     * @brief 析构函数
     * 
     * 定义虚的析构函数，保证派生类析构的时候能执行正确的析构函数，使得
     * 派生类能够在对象被销毁的时候正确的清理占用的内存
    */
    virtual ~Value() = default;

    /**
     * @brief 获取值的数据类型
     * 
     * @return 返回枚举类型中这个对象对应的数据类型
     * 
     * @note 这个函数是一个制度函数，执行的时候不改变对象的内容
    */
    ValueType getType() const {
        return type;
    }

    /**
     * @brief 这是一个将 Value 对象转换为字符串的函数，因为我们经常需要输出值，需要进行转换为外部表示
     * 
     * @return 返回的是 string 类型，将对象转换为字符串，具体的就是布尔类型会转换为 #f 或者 #t，
     *         数类型和字符串都是直接转换为对应的字符串，空表转换为 () ，对子比较复杂，用到递归。
     * 
     * @note 这个函数是个纯虚函数，不同类型的值会有不同的得到外部表示的方法，派生类必须实现这个函数
     * 
     * @note 这个函数是一个只读函数，执行过程不会改变对象的内容
    */
    virtual std::string toString() const = 0;

    /**
     * @brief 这是一个判断值是否为空表类型的函数
     * 
     * @return 返回的是布尔类型，如果值得类型是空表，则返回 true ，否则返回 false 。
     * 
     * @note 这是一个只读函数，不改变对象的内容
    */
    bool isNil() const;

    /**
     * @brief 这是一个判断值是否为自求值类型的函数
     * 
     * @return 返回的是 bool 类型，如果值是布尔类型、数类型或字符串类型，返回 true ，否则返回 false 。
     * 
     * @note 这是一个只读函数，不改变对象的内容
    */
    bool isSelfEvaluating() const;

    /**
     * @brief 这是一个判断值是否为对子的函数
     * 
     * @return 返回的是 bool 类型，如果值是对子类型，则返回 true ，否则返回 false 。
     * 
     * @note 这是一个只读函数，不改变对象的内容
    */
    bool isList() const;

    /**
     * @brief 这是一个判断值是否为符号的函数
     * 
     * @return 返回的是 bool 类型，如果值是符号类型，则返回 true ，否则返回 false 。
     * 
     * @note 这是一个只读函数，不改变对象的内容
    */
    bool isSymbol() const;

    bool isNumber() const;

    /**
     * @brief 这个函数用来获取值得符号
     * 
     * @return 如果这个值是一个符号，则返回符号的 name ，否则返回 std::nullopt
     * 
     * @note 这是一个虚函数，调用时会判断调用对象的类型
     * 
     * @note 这是一个只读函数，执行时不改变对象的内容
    */
    virtual std::optional<std::string> asSymbol() const;

    virtual std::optional<double> asNumber() const;

    virtual std::vector<ValuePtr> toVector() const = 0;
};

/**
 * @brief 这是布尔类型值的类，表示 Mini-Lisp 中的布尔类型，是基类 Value 的派生类
 * 
 * 布尔类型值类用于表示 Mini-Lisp 中的布尔类型数据。它继承自基类 Value，因而具有基类定义的通用接口和行为。
 * 
 * 这个类定义了布尔类型值的属性和行为：
 * - value 属性存储布尔值本身。
 * - 提供了构造函数用于直接构造和复制构造布尔类型值。
 * - 提供了获取布尔值的接口 getValue()。
 * - 重写了基类的纯虚函数 toString()，用于获取布尔值的外部表示。
 * 
 * @note 这个类实现了基类 Value 的纯虚函数 toString()，因此可以用于创建对象。
 * @note 这个类没有重载基类的虚函数 asSymbol()，仍使用基类的函数。
 * @note 删除了默认构造函数，因为布尔值类型必须具有明确的布尔值来构造对象，不允许无参数构造函数。
*/
class BooleanValue : public Value {
private:
    bool value; // 存储布尔值的属性

public:
    /**
     * @brief 删除的默认构造函数
     * 
     * 布尔类型值对象必须具有明确的布尔值来构造，因此禁止无参数的默认构造函数。
    */
    BooleanValue() = delete;

    /**
     * @brief 构造函数，用给定的布尔值创建 BooleanValue 对象
     * 
     * @param value 布尔值
    */
    BooleanValue(bool value) : Value{ValueType::BOOLEAN_VALUE}, value{value} {}

    /**
     * @brief 复制构造函数，从另一个 BooleanValue 对象复制布尔值
     * 
     * @param b 要复制的 BooleanValue 对象
    */
    BooleanValue(const BooleanValue& b) : Value{b}, value{b.value} {}

    /**
     * @brief 虚析构函数
     * 
     * 定义虚的析构函数，保证在对象被销毁时，正确调用派生类的析构函数，释放相关资源。
    */
    virtual ~BooleanValue() = default;

    /**
     * @brief 获取布尔值的函数
     * 
     * @return 返回布尔值
    */
    bool getValue() const {
        return value;
    }

    /**
     * @brief 获取布尔值的外部表示的函数
     * 
     * @return 返回布尔值的字符串表示，可以是 "#t" 表示真，或 "#f" 表示假
     * 
     * @note 这是从基类继承来的函数，对基类的 toString() 进行了重载，实现了基类的纯虚函数
    */
    virtual std::string toString() const override;

    virtual std::vector<ValuePtr> toVector() const override;
};

/**
 * @brief 这是数值类型值的类，表示 Mini-Lisp 中的数值类型，是基类 Value 的派生类
 * 
 * 数值类型值类用于表示 Mini-Lisp 中的数值类型数据。它派生自基类 Value，因此具有基类定义的通用接口和行为。
 * 
 * 这个类定义了数值类型值的属性和行为：
 * - value 属性存储数值本身。
 * - 提供了构造函数用于直接构造和复制构造数值类型值。
 * - 提供了获取数值的接口 getValue()。
 * - 重写了基类的纯虚函数 toString()，用于获取数值的外部表示。
 * 
 * @note 这个类实现了基类 Value 的纯虚函数 toString()，因此可以用于创建对象。
 * @note 这个类没有重载基类的虚函数 asSymbol()，仍使用基类的函数。
 * @note 删除了默认构造函数，因为数值类型必须具有明确的数值来构造对象，不允许无参数构造函数。
*/
class NumericValue : public Value {

private:
    double value; // 存储数值

public:
    /**
     * @brief 删除的默认构造函数
     * 
     * 数值类型值对象必须具有明确的数值来构造，因此禁止无参数的默认构造函数。
    */
    NumericValue() = delete;

    /**
     * @brief 构造函数，用给定的数值创建 NumericValue 对象
     * 
     * @param value 数值
    */
    NumericValue(double value) : Value{ValueType::NUMERIC_VALUE}, value{value} {}

    /**
     * @brief 复制构造函数，从另一个 NumericValue 对象复制数值
     * 
     * @param n 要复制的 NumericValue 对象
    */
    NumericValue(const NumericValue& n) : Value{n}, value{n.value} {}

    /**
     * @brief 虚析构函数
     * 
     * 定义虚的析构函数，保证在对象被销毁时，正确调用派生类的析构函数，释放相关资源。
    */
    virtual ~NumericValue() = default;

    /**
     * @brief 获取数值的函数
     * 
     * @return 返回数值
    */
    double getValue() const {
        return value;
    }

    /**
     * @brief 获取数值的外部表示的函数
     * 
     * @return 返回数值的字符串表示
     * 
     * @note 这是从基类继承来的函数，对基类的 toString() 进行了重载，实现了基类的纯虚函数
    */
    virtual std::string toString() const override;

    virtual std::optional<double> asNumber() const override;

    virtual std::vector<ValuePtr> toVector() const override;
};

/**
 * @brief 这是字符串类型值的类，表示 Mini-Lisp 中的字符串类型，是基类 Value 的派生类
 * 
 * 字符串类型值类用于表示 Mini-Lisp 中的字符串类型数据。它派生自基类 Value，因此具有基类定义的通用接口和行为。
 * 
 * 这个类定义了字符串类型值的属性和行为：
 * - value 属性存储字符串本身。
 * - 提供了构造函数用于直接构造和复制构造字符串类型值。
 * - 提供了获取字符串的接口 getValue()。
 * - 重写了基类的纯虚函数 toString()，用于获取字符串的外部表示。
 * 
 * @note 这个类实现了基类 Value 的纯虚函数 toString()，因此可以用于创建对象。
 * @note 这个类没有重载基类的虚函数 asSymbol()，仍使用基类的函数。
 * @note 删除了默认构造函数，因为字符串类型必须具有明确的字符串来构造对象，不允许无参数构造函数。
*/
class StringValue : public Value {

private:
    std::string value; // 存储字符串

public:
    /**
     * @brief 删除的默认构造函数
     * 
     * 字符串类型值对象必须具有明确的字符串来构造，因此禁止无参数的默认构造函数。
    */
    StringValue() = delete;


    /**
     * @brief 构造函数，用给定的字符串创建 StringValue 对象
     * 
     * @param value 字符串
    */
    StringValue(const std::string& value) : Value{ValueType::STRING_VALUE}, value{value} {}

    /**
     * @brief 复制构造函数，从另一个 StringValue 对象复制字符串
     * 
     * @param s 要复制的 StringValue 对象
    */
    StringValue(const StringValue& s) : Value{s}, value{s.value} {}

    /**
     * @brief 虚析构函数
     * 
     * 定义虚的析构函数，保证在对象被销毁时，正确调用派生类的析构函数，释放相关资源。
    */
    virtual ~StringValue() = default;


    /**
     * @brief 获取字符串的函数
     * 
     * @return 返回字符串
    */
    const std::string& getValue() const {
        return value;
    }

    /**
     * @brief 获取字符串的外部表示的函数
     * 
     * @return 返回字符串的字符串表示
     * 
     * @note 这是从基类继承来的函数，对基类的 toString() 进行了重载，实现了基类的纯虚函数
    */
    virtual std::string toString() const override;

    virtual std::vector<ValuePtr> toVector() const override;
};

/**
 * @brief 这是空表类型值的类，表示 Mini-Lisp 中的空表类型，是基类 Value 的派生类
 * 
 * 空表类型值类用于表示 Mini-Lisp 中的空表类型数据。它派生自基类 Value，因此具有基类定义的通用接口和行为。
 * 
 * 这个类定义了空表类型值的行为：
 * - 提供了构造函数用于直接构造空表类型值。
 * - 提供了复制构造函数用于从另一个空表类型值对象复制数据。
 * - 重写了基类的纯虚函数 toString()，用于获取空表的外部表示。
 * 
 * @note 这个类实现了基类 Value 的纯虚函数 toString()，因此可以用于创建对象。
 * @note 这个类没有重载基类的虚函数 asSymbol()，仍使用基类的函数。
*/
class NilValue : public Value {
public:
    /**
     * @brief 构造函数，用于创建空表类型值对象
    */
    NilValue() : Value{ValueType::NIL_VALUE} {}

    /**
     * @brief 复制构造函数，从另一个 NilValue 对象复制数据
     * 
     * @param n 要复制的 NilValue 对象
    */
    NilValue(const NilValue& n) : Value(n) {}

    /**
     * @brief 虚析构函数
     * 
     * 定义虚的析构函数，保证在对象被销毁时，正确调用派生类的析构函数，释放相关资源。
    */
    virtual ~NilValue() = default;

    /**
     * @brief 获取空表的外部表示的函数
     * 
     * @return 返回空表的字符串表示，即 "()"
     * 
     * @note 这是从基类继承来的函数，对基类的 toString() 进行了重载，实现了基类的纯虚函数
    */
    virtual std::string toString() const override;

    virtual std::vector<ValuePtr> toVector() const override;
};

/**
 * @brief 这是符号类型值的类，表示 Mini-Lisp 中的符号类型，是基类 Value 的派生类
 * 
 * 符号类型值类用于表示 Mini-Lisp 中的符号类型数据。它派生自基类 Value，因此具有基类定义的通用接口和行为。
 * 
 * 这个类定义了符号类型值的属性和行为：
 * - name 属性存储符号的名称。
 * - 提供了构造函数用于直接构造符号类型值。
 * - 提供了获取符号名称的接口 getName()。
 * - 重写了基类的纯虚函数 toString()，用于获取符号的外部表示。
 * - 重写了基类的虚函数 asSymbol()，用于获取符号的名称。
 * 
 * @note 这个类实现了基类 Value 的纯虚函数 toString() 和 asSymbol()，因此可以用于创建对象。
 * @note 删除了默认构造函数，因为符号类型必须具有明确的名称来构造对象，不允许无参数构造函数。
*/
class SymbolValue : public Value {

private:
    std::string name; // 存储符号的名称

public:
    /**
     * @brief 删除的默认构造函数
     * 
     * 符号类型值对象必须具有明确的名称来构造，因此禁止无参数的默认构造函数。
    */
    SymbolValue() = delete;

    /**
     * @brief 构造函数，用给定的名称创建 SymbolValue 对象
     * 
     * @param name 符号的名称
    */
    SymbolValue(const std::string& name) : Value{ValueType::SYMBOL_VALUE}, name{name} {}


    /**
     * @brief 虚析构函数
     * 
     * 定义虚的析构函数，保证在对象被销毁时，正确调用派生类的析构函数，释放相关资源。
    */
    virtual ~SymbolValue() = default;

    /**
     * @brief 获取符号的名称
     * 
     * @return 返回符号的名称
    */
    const std::string& getName() const {
        return name;
    }

    /**
     * @brief 获取符号的外部表示的函数
     * 
     * @return 返回符号的字符串表示，即其名称
     * 
     * @note 这是从基类继承来的函数，对基类的 toString() 进行了重载，实现了基类的纯虚函数
    */
    virtual std::string toString() const override;


    /**
     * @brief 获取符号的名称
     * 
     * @return 返回符号的名称作为字符串
     * 
     * @note 这是从基类继承来的函数，对基类的 asSymbol() 进行了重载，实现了基类的虚函数
    */
    virtual std::optional<std::string> asSymbol() const override;

    virtual std::vector<ValuePtr> toVector() const override;
};

/**
 * @brief 这是对子类型值的类，表示 Mini-Lisp 中的对子类型，是基类 Value 的派生类
 * 
 * 对子类型值类用于表示 Mini-Lisp 中的对子类型数据。它派生自基类 Value，因此具有基类定义的通用接口和行为。
 * 
 * 这个类定义了对子类型值的属性和行为：
 * - left 和 right 属性存储对子的左右元素。
 * - 提供了构造函数用于直接构造对子类型值。
 * - 提供了获取对子值的接口 getValue()。
 * - 提供了获取对子的左右元素的接口 getLeft() 和 getRight()。
 * - 重写了基类的纯虚函数 toString()，用于获取对子的外部表示。
 * - 提供了将对子转换为标准容器的函数 toVector()，便于对子的遍历和处理。
 * 
 * @note 这个类实现了基类 Value 的纯虚函数 toString()，因此可以用于创建对象。
 * @note 删除了默认构造函数，因为对子类型必须具有明确的左右元素来构造对象，不允许无参数构造函数。
*/
class PairValue : public Value {

private:
    std::shared_ptr<Value> left;  // 存储对子的左元素
    std::shared_ptr<Value> right; // 存储对子的右元素

public:
    /**
     * @brief 删除的默认构造函数
     * 
     * 对子类型值对象必须具有明确的左右元素来构造，因此禁止无参数的默认构造函数。
    */
    PairValue() = delete;

    /**
     * @brief 构造函数，用给定的左右元素创建 PairValue 对象
     * 
     * @param left 对子的左元素
     * @param right 对子的右元素
    */
    PairValue(const ValuePtr& left, const ValuePtr& right) : Value{ValueType::PAIR_VALUE}, left{left}, right{right} {}

    /**
     * @brief 复制构造函数，从另一个 PairValue 对象复制对子的左右元素
     * 
     * @param p 要复制的 PairValue 对象
    */
    PairValue(const PairValue& p) : Value{p}, left{p.left}, right{p.right} {}

    /**
     * @brief 虚析构函数
     * 
     * 定义虚的析构函数，保证在对象被销毁时，正确调用派生类的析构函数，释放相关资源。
    */
    virtual ~PairValue() = default;

    /**
     * @brief 获取对子的左右元素
     * 
     * @return 返回对子的左右元素，以 std::pair 形式返回
    */
    const auto getValue() const {
        return std::make_pair(left, right);
    }

    /**
     * @brief 获取对子的左元素
     * 
     * @return 返回对子的左元素
    */
    auto& getLeft() const {
        return left;
    }

    /**
     * @brief 获取对子的右元素
     * 
     * @return 返回对子的右元素
    */
    auto& getRight() const {
        return right;
    }

    /**
     * @brief 获取对子的外部表示的函数
     * 
     * @return 返回对子的字符串表示
     * 
     * @note 这是从基类继承来的函数，对基类的 toString() 进行了重载，实现了基类的纯虚函数
    */
    virtual std::string toString() const override;

    virtual std::vector<ValuePtr> toVector() const override;
};

class EvalEnv;

using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

class BuiltinProcValue : public Value {
private:
    BuiltinFuncType* func;

public:
    BuiltinProcValue(BuiltinFuncType* ptr) : Value{ValueType::BUILTIN_PROC_VALUE}, func{ptr} {}

    virtual std::string toString() const override;

    BuiltinFuncType* getFunction() const {
        return func;
    }

    virtual std::vector<ValuePtr> toVector() const override;
};

class LambdaValue : public Value {
private:
    std::vector<std::string> params;
    std::vector<ValuePtr> body;
    std::shared_ptr<EvalEnv> parent;

public:
    LambdaValue(const std::vector<std::string>& params, const std::vector<ValuePtr>& body, std::shared_ptr<EvalEnv> env);

    virtual std::string toString() const override;

    virtual std::vector<ValuePtr> toVector() const override;

    ValuePtr apply(const std::vector<ValuePtr>& args);
};

#endif  // VALUE_H
