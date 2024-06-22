#ifndef __PARAMS_CHECKER_H__
#define __PARAMS_CHECKER_H__

#include "value.h"
#include "error.h"
#include "assert.h"
#include <algorithm>

/**
 * @brief 抽象基类 ParamsNumberChecker，用于检查参数数量。
 * 
 * @details 派生类应实现 `numberCheck` 函数来验证参数数量是否符合预期。
 */
class ParamsNumberChecker {
public:
    /**
     * @brief 检查参数数量是否符合预期。
     * 
     * @param params 参数列表
     * @param name 函数名称
     * @throws LispError 如果参数数量不匹配
     */
    virtual void numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const = 0;
};

/**
 * @brief 派生类 EqualParamsNumberChecker，用于检查参数数量是否与指定值相等。
 * 
 * @details 通过构造函数指定预期的参数数量，然后在 `numberCheck` 函数中验证实际参数数量是否相等。
 */
class EqualParamsNumberChecker : public ParamsNumberChecker {
private:
    int n;
public:
    /**
     * @brief 构造函数，指定预期的参数数量。
     * 
     * @param n 预期的参数数量
     */
    EqualParamsNumberChecker(int n) : ParamsNumberChecker(), n{n} {}

    /**
     * @brief 检查参数数量是否与预期相等。
     * 
     * @param params 参数列表
     * @param name 函数名称
     * @throws LispError 如果参数数量不匹配
     */
    void numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const override;
};

/**
 * @brief 派生类 LargerParamsNumberChecker，用于检查参数数量是否大于指定值。
 * 
 * @details 通过构造函数指定预期的参数数量，然后在 `numberCheck` 函数中验证实际参数数量是否大于预期值。
 */
class LargerParamsNumberChecker : public ParamsNumberChecker {
private:
    int n;
public:
    /**
     * @brief 构造函数，指定预期的参数数量。
     * 
     * @param n 预期的参数数量
     */
    LargerParamsNumberChecker(int n) : ParamsNumberChecker(), n{n} {}

    /**
     * @brief 检查参数数量是否大于预期。
     * 
     * @param params 参数列表
     * @param name 函数名称
     * @throws LispError 如果参数数量不匹配
     */
    void numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const override;
};

/**
 * @brief 派生类 LesserParamsNumberChecker，用于检查参数数量是否小于指定值。
 * 
 * @details 通过构造函数指定预期的参数数量，然后在 `numberCheck` 函数中验证实际参数数量是否小于预期值。
 */
class LesserParamsNumberChecker : public ParamsNumberChecker {
private:
    int n;
public:
    /**
     * @brief 构造函数，指定预期的参数数量。
     * 
     * @param n 预期的参数数量
     */
    LesserParamsNumberChecker(int n) : ParamsNumberChecker(), n{n} {}

    /**
     * @brief 检查参数数量是否小于预期。
     * 
     * @param params 参数列表
     * @param name 函数名称
     * @throws LispError 如果参数数量不匹配
     */
    void numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const override;
};

/**
 * @brief 派生类 RangeParamsNumberChecker，用于检查参数数量是否在指定范围内。
 * 
 * @details 通过构造函数指定预期的参数数量范围，然后在 `numberCheck` 函数中验证实际参数数量是否在该范围内。
 */
class RangeParamsNumberChecker : public ParamsNumberChecker {
private:
    std::vector<int> numbers;

    /**
     * @brief 获取参数数量范围的字符串表示。
     * 
     * @return 参数数量范围的字符串表示
     * @throws std::runtime_error 如果范围为空
     */
    std::string get_string() const;
public:
    /**
     * @brief 构造函数，指定预期的参数数量范围。
     * 
     * @param numbers 参数数量范围
     */
    RangeParamsNumberChecker(const std::vector<int>& numbers) : ParamsNumberChecker(), numbers{numbers} {}

    /**
     * @brief 检查参数数量是否在预期范围内。
     * 
     * @param params 参数列表
     * @param name 函数名称
     * @throws LispError 如果参数数量不匹配
     */
    void numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const override;
};

/**
 * @brief 派生类 ParamsChecker，用于综合检查参数的数量和类型是否符合预期。
 * 
 * @details 通过构造函数指定参数列表、函数名称和参数数量检查器，然后可以添加参数类型检查器，并在 `check` 函数中执行综合检查。
 */
class ParamTypeChecker {
private:
    std::vector<ValueType> types;
    int n;

    std::string get_string() const;
public:
    /**
     * @brief 构造函数，指定所需的参数类型列表和参数索引。
     * 
     * @param types 所需的参数类型列表
     * @param n 参数索引
     */
    ParamTypeChecker(const std::vector<ValueType>& types, int n) : types{types}, n{n} {}

    /**
     * @brief 检查参数的类型是否符合预期。
     * 
     * @param params 参数列表
     * @param name 函数名称
     * @throws LispError 如果参数类型不匹配
     */
    void checkType(const std::vector<ValuePtr>& ptr, const std::string& name);
};

class ParamsChecker {
private:
    const std::vector<ValuePtr>& params;
    std::string name;
    std::unique_ptr<ParamsNumberChecker> numberChecker;
    std::vector<std::shared_ptr<ParamTypeChecker>> typeCheckers;
public:
    /**
     * @brief 构造函数，指定参数列表、函数名称和参数数量检查器。
     * 
     * @param params 参数列表
     * @param name 函数名称
     * @param numberChecker 参数数量检查器
     */
    ParamsChecker(const std::vector<ValuePtr>& params, const std::string& name, std::unique_ptr<ParamsNumberChecker> numberChecker);

    /**
     * @brief 向类型检查器列表中添加一个 ParamTypeChecker 对象。
     * 
     * @param n 参数索引
     * @param requiredTypes 所需的参数类型列表
     */
    void addTypeRequire(int n, const std::vector<ValueType>& requiredTypes);

    /**
     * @brief 综合检查参数的数量和类型是否符合预期。
     * 
     * @throws LispError 如果参数数量或类型不匹配
     */
    void check();
};

#endif