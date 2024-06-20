#ifndef __PARAMS_CHECKER_H__
#define __PARAMS_CHECKER_H__

#include "value.h"
#include "error.h"
#include "assert.h"
#include <algorithm>

class ParamsNumberChecker {
public:
    virtual void numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const = 0;
};

class EqualParamsNumberChecker : public ParamsNumberChecker {
private:
    int n;
public:
    EqualParamsNumberChecker(int n) : ParamsNumberChecker(), n{n} {}

    void numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const override;
};

class LargerParamsNumberChecker : public ParamsNumberChecker {
private:
    int n;
public:
    LargerParamsNumberChecker(int n) : ParamsNumberChecker(), n{n} {}

    void numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const override;
};

class LesserParamsNumberChecker : public ParamsNumberChecker {
private:
    int n;
public:
    LesserParamsNumberChecker(int n) : ParamsNumberChecker(), n{n} {}

    void numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const override;
};

class RangeParamsNumberChecker : public ParamsNumberChecker {
private:
    std::vector<int> numbers;

    std::string get_string() const;
public:
    RangeParamsNumberChecker(const std::vector<int>& numbers) : ParamsNumberChecker(), numbers{numbers} {}

    void numberCheck(const std::vector<ValuePtr>& params, const std::string& name) const override;
};

class ParamTypeChecker {
private:
    const ValueType type;
    int n;
public:
    ParamTypeChecker(const ValueType type, int n) : type{type}, n{n} {}

    void checkType(const std::vector<ValuePtr>& ptr, const std::string& name);
};

class ParamsChecker {
private:
    const std::vector<ValuePtr>& params;
    std::string name;
    std::unique_ptr<ParamsNumberChecker> numberChecker;
    std::vector<std::shared_ptr<ParamTypeChecker>> typeCheckers;
public:
    ParamsChecker(const std::vector<ValuePtr>& params, const std::string& name, std::unique_ptr<ParamsNumberChecker> numberChecker);

    void addTypeRequire(int n, const ValueType type);

    void check();
};

#endif