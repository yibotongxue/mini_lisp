#include "value.h"
#include "forms.h"
#include "builtins.h"
#include "error.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include "eval_env.h"

LambdaValue::LambdaValue(const std::vector<std::string>& params, const std::vector<ValuePtr>& body, std::shared_ptr<EvalEnv> env)
    : Value{ValueType::LAMBDA_VALUE}, params{params}, body{body}, parent{env} {}

/**
 * @brief 判断值是否为空表类型的函数
 * 
 * @return 如果值的类型是空表类型，则返回 true；否则返回 false。
 * 
 * @note 这是一个只读函数，不会改变对象的内容。
*/
bool Value::isNil() const {
    if(type == ValueType::NIL_VALUE)
        return true;
    return false;
}

/**
 * @brief 判断值是否为自求值类型的函数
 * 
 * @return 如果值的类型是布尔类型、数值类型或字符串类型，则返回 true；否则返回 false。
 * 
 * @note 这是一个只读函数，不会改变对象的内容。
*/
bool Value::isSelfEvaluating() const {
    if(type == ValueType::BOOLEAN_VALUE ||
       type == ValueType::NUMERIC_VALUE ||
       type == ValueType::STRING_VALUE ||
       type == ValueType::BUILTIN_PROC_VALUE || 
       type == ValueType::LAMBDA_VALUE)
        return true;
    return false;
}

namespace{
/**
 * @brief 递归函数，用于获取对子中最右侧的值。
 * 
 * 如果传入的指针不是对子类型，直接返回该指针；否则，递归地获取对子的右侧值。
 * 
 * @param ptr 要处理的Value指针
 * @return 如果ptr不是对子类型，返回ptr；否则返回对子的最右侧值。
 */
Value* getRightOne(Value* ptr) {
    if (ptr->getType() != ValueType::PAIR_VALUE) {
        return ptr;
    }
    else {
        return getRightOne(dynamic_cast<PairValue*>(ptr)->getRight().get());
    }
}
}

/**
 * @brief 判断值是否为对子类型的函数
 * 
 * @return 如果值的类型是对子类型，则返回 true；否则返回 false。
 * 
 * @note 这是一个只读函数，不会改变对象的内容。
*/
bool Value::isList() const {
    if(getRightOne(const_cast<Value*>(this))->getType() == ValueType::NIL_VALUE && !isNil())
        return true;
    return false;
}

/**
 * @brief 判断值是否为符号类型的函数
 * 
 * @return 如果值的类型是符号类型，则返回 true；否则返回 false。
 * 
 * @note 这是一个只读函数，不会改变对象的内容。
*/
bool Value::isSymbol() const {
    if(type == ValueType::SYMBOL_VALUE)
        return true;
    return false;
}

/**
 * @brief 判断值是否为数类型的函数
 * 
 * @return 如果值的类型是数类型，则返回 true；否则返回 false。
 * 
 * @note 这是一个只读函数，不会改变对象的内容。
 */
bool Value::isNumber() const {
    if (type == ValueType::NUMERIC_VALUE) 
        return true;
    return false;
}

/**
 * @brief 将值作为符号返回，这里处理不是符号的情况
 * 
 * @return 返回 std::nullopt
*/
std::optional<std::string> Value::asSymbol() const {
   return std::nullopt;
}

/**
 * @brief 将值作为符号返回，这里处理值类型就是符号类型的情况
 * 
 * @return 返回用符号名称构造的 std::optional<std::string> 对象
*/
std::optional<std::string> SymbolValue::asSymbol() const {
    return { name };
}

/**
 * @brief 将值作为数返回，这里处理不是数的情况
 * 
 * @return 返回 std::nullopt
*/
std::optional<double> Value::asNumber() const {
    return std::nullopt;
}

/**
 * @brief 将值作为数返回，这里处理值类型就是数类型的情况
 * 
 * @return 返回用数的值构造的 std::optional<double> 对象
*/
std::optional<double> NumericValue::asNumber() const {
    return { value };
}

/**
 * @brief 获取布尔值的外部表示的函数
 * 
 * @return 返回布尔值的字符串表示，可以是 "#t" 表示真，或 "#f" 表示假
*/
std::string BooleanValue::toString() const {
    return value? "#t" : "#f";
}

/**
 * @brief 获取数值的外部表示的函数
 * 
 * @return 返回数值的字符串表示，如果是整数，则直接返回整数的字符串，否则返回带小数点的浮点数的字符串
*/
std::string NumericValue::toString() const {
    if(int(value) == value)
        return std::to_string(int(value));
    return std::to_string(value);
}

/**
 * @brief 获取字符串的外部表示的函数
 * 
 * @return 返回加引号的字符串
*/
std::string StringValue::toString() const {
    std::stringstream ss;
    ss << std::quoted(value);
    return ss.str();
}

/**
 * @brief 获取空表的外部表示的函数
 * 
 * @return 返回一堆括号的字符串
*/
std::string NilValue::toString() const {
    return "()";
}

/**
 * @brief 获取符号的外部表示的函数
 * 
 * @return 返回符号的名称
*/
std::string SymbolValue::toString() const {
    return name;
}

namespace{
    /**
     * @brief 解析值并构造字符串表示的函数
    * 
    * @param result 用于存储解析后的字符串结果的向量
     * @param ref 要解析的值的引用
    * @return 返回解析后的值的引用
    * 
    * @note 该函数递归地解析值并构造字符串表示。如果值是一个 PairValue，则递归地解析其左右部分。
    *       如果值不是 PairValue，则将其转换为字符串并存储在结果向量中。
    */
    const Value& pharase(std::vector<std::string>& result, const Value& ref) {
        if(typeid(ref) == typeid(PairValue&)) {
            const PairValue& r = dynamic_cast<const PairValue&>(ref);
            result.push_back(r.getLeft()->toString());
          const Value& reff = pharase(result, *(r.getRight())); // 递归解析右侧值
          return reff;
      }
      else {
            result.push_back(ref.toString()); // 将非 PairValue 类型值转换为字符串并存储
     }
      return ref; // 返回解析后的值的引用
    }
};

/**
 * @brief 获取 PairValue 对象的外部表示的函数
 * 
 * @return 返回 PairValue 对象的字符串表示
 * 
 * @note 该函数使用解析函数 `pharase()` 解析 PairValue 对象，并根据其内容构造字符串表示。
 *       如果 PairValue 对象的右部分是 NilValue，则结果形如 "(value1 value2 ...)"；
 *       否则形如 "(value1 value2 ... . valueN)"
 */
std::string PairValue::toString() const {
    std::vector<std::string> vec{};
    const Value& ref = dynamic_cast<const Value&>(*this); // 引用基类对象
    const Value& v = pharase(vec, ref); // 解析 PairValue 对象
    std::string result = "(";
    if(typeid(v) == typeid(NilValue&)) { // 检查右侧是否为 NilValue
        for(int i = 0; i < vec.size() - 1; i++) {
            result += (vec[i] + " "); // 构造左侧部分
        }
        result.back() = ')'; // 添加右括号
    }
    else {
        for(int i = 0; i < vec.size() - 1; i++) {
            result += vec[i] + " "; // 构造左侧部分
        }
        result += ". "; // 添加点号
        result += vec[vec.size() - 1] + ")"; // 添加右侧部分
    }
    return result; // 返回字符串表示
}

/**
 * @brief 获取内置过程的外部表示的函数
 * 
 * @return 返回 #<procedure>
*/
std::string BuiltinProcValue::toString() const {
    return "#<procedure>";
}

/**
 * @brief 获取 lambda 表达式的外部表示的函数
 * 
 * @return 返回 #<procedure>
*/
std::string LambdaValue::toString() const {
    return "#<procedure>";
}

/**
 * @brief 将BooleanValue转换为包含自身的向量。
 * 
 * @return 含有BooleanValue的向量
 */
std::vector<ValuePtr> BooleanValue::toVector() const {
    return { std::make_shared<BooleanValue>(*this) };
}

/**
 * @brief 将NumericValue转换为包含自身的向量。
 * 
 * @return 含有NumericValue的向量
 */
std::vector<ValuePtr> NumericValue::toVector() const {
    return { std::make_shared<NumericValue>(*this) };
}

/**
 * @brief 将StringValue转换为包含自身的向量。
 * 
 * @return 含有StringValue的向量
 */
std::vector<ValuePtr> StringValue::toVector() const {
    return { std::make_shared<StringValue>(*this) };
}

/**
 * @brief 将NilValue转换为空向量。
 * 
 * @return 空向量
 */
std::vector<ValuePtr> NilValue::toVector() const {
    return {};
}

/**
 * @brief 将SymbolValue转换为包含自身的向量。
 * 
 * @return 含有SymbolValue的向量
 */
std::vector<ValuePtr> SymbolValue::toVector() const {
    return { std::make_shared<SymbolValue>(*this) };
}

namespace{
/**
 * @brief 递归函数，用于构建一个列表（PairValue）。
 * 
 * @param vec 包含要构建列表的元素的向量
 * @param p 当前处理的值
 */
void backtracking(std::vector<ValuePtr>& vec, ValuePtr p) {
    if (p->isList()) {
        auto pairP = std::dynamic_pointer_cast<PairValue>(p);
        vec.push_back(pairP->getLeft());
        backtracking(vec, pairP->getRight());
    }
    else {
        vec.push_back(p);
    }
}
}

/**
 * @brief 将PairValue转换为包含自身的向量。
 * 
 * @return 含有PairValue的向量
 */
std::vector<ValuePtr> PairValue::toVector() const {
    if (isList()) {
        std::vector<ValuePtr> result{};
        result.push_back(left);
        backtracking(result, right);
        return result;
    }
    else {
        return { std::make_shared<PairValue>(*this) };
    }
}

/**
 * @brief 将BuiltinProcValue转换为包含自身的向量。
 * 
 * @return 含有BuiltinProcValue的向量
 */
std::vector<ValuePtr> BuiltinProcValue::toVector() const {
    return { std::make_shared<BuiltinProcValue>(*this) };
}

/**
 * @brief 将LambdaValue转换为包含自身的向量。
 * 
 * @return 含有LambdaValue的向量
 */
std::vector<ValuePtr> LambdaValue::toVector() const {
    return { std::make_shared<LambdaValue>(*this) };
}

/**
 * @brief 执行Lambda函数，应用参数并返回结果。
 * 
 * @param args 参数列表
 * @return Lambda函数的执行结果
 */
ValuePtr LambdaValue::apply(const std::vector<ValuePtr>& args) {
    auto env = parent->createChild(params, args);
    if (body.empty()) {
        return std::make_shared<NilValue>();
    }
    for (int i = 0; i < static_cast<int>(body.size()) - 1; i++) {
        env->eval(body[i]);
    }
    return env->eval(body.back());
}
