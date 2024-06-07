/* ************************************************************************
> File Name:     builtins.cpp
> Author:        林毅波
> Created Time:  2024/5/23 21:26:41
> Description:   
 ************************************************************************/

#include "..\include\builtins.h"
#include "../include/error.h"
#include "../include/eval_env.h"
#include <iostream>
#include <cmath>
#include <iterator>
#include <algorithm>

namespace{
    void check_n_params(const std::vector<ValuePtr>& params, int n, const std::string& procedure) {
        if (params.size() != n) {
            throw LispError("The " + procedure + " procedure need " + std::to_string(n) + " params, given " + std::to_string(int(params.size())) + ".");
        }
    }

    void check_n_params_type(const std::vector<ValuePtr>& params, int n, const std::string& procedure, ValueType& type) {
        if (params.size() != n) {
            throw LispError("The " + procedure + " procedure need " + std::to_string(n) + " params, given " + std::to_string(int(params.size())) + ".");
        }
        for (int i = 0; i < params.size(); i++) {
            if (params[i]->getType() != type) {
                throw LispError("The " + procedure + " procedure cannot receive a non value.");
            }
        }
    }

    void check_two_numbers(const std::vector<ValuePtr>& params, const std::string& procedure) {
        check_n_params(params, 2, procedure);
        if (!params[0]->isNumber() || !params[1]->isNumber()) {
            throw LispError("The " + procedure + " procedure cannot receive a non-numeric value.");
        }
    }
    
    void check_one_number(const std::vector<ValuePtr>& params, const std::string& procedure) {
        check_n_params(params, 1, procedure);
        if (!params[0]->isNumber()) {
            throw LispError("The " + procedure + " procedure cannot receive a non-numeric value.");
        }
    }
}

ValuePtr add(const std::vector<ValuePtr>& params, EvalEnv&) {
    double result = 0;
    for (const auto& i :params) {
        if (i->isNumber()) {
            result += *i->asNumber();
        }
        else {
            throw LispError("Cannot add a non-numeric value.");
        }
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv&) {
    for (const auto& param : params) {
        std::cout << param->toString();
    }
    return std::make_shared<NilValue>();
}

ValuePtr multiply(const std::vector<ValuePtr>& params, EvalEnv&) {
    double result = 1;
    for (const auto& i : params) {
        if (i->isNumber()) {
            result *= *i->asNumber();
        }
        else {
            throw LispError("Cannot multiply a non-numeric value.");
        }
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr reduce(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("Less params than needed.");
    }
    else if (params.size() == 1) {
        if (params[0]->isNumber()) {
            return std::make_shared<NumericValue>(-*params[0]->asNumber());
        }
        else {
            throw LispError("Cannot reduce a non-numeric value.");
        }
    }
    else {
        if (params[0]->isNumber()) {
            double result = *params[0]->asNumber();
            for (int i = 1; i < params.size(); i++) {
                if (params[i]->isNumber()) {
                    result -= *params[i]->asNumber();
                }
                else {
                    throw LispError("Cannot reduce a non-numeric value.");
                }
            }
            return std::make_shared<NumericValue>(result);
        }
        else {
            throw LispError("Cannot reduce a non-numeric value.");
        }
    }
}

ValuePtr apply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    check_n_params(params, 2, "apply");
    // if (params.size() == 1) {
    //     throw LispError("A param is needed.");
    // }
    // if (params.size() > 2) {
    //     throw LispError("The params are more than needed.");
    // }
    if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
        if (params[1]->isList() || params[1]->isNil()) {
            auto vec = params[1]->toVector();
            vec.pop_back();
            return std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()(vec, env);
        }
        else {
            throw LispError("expected a list that can be applied on a procedure");
        }
    }
    else if (params[0]->getType() == ValueType::LAMBDA_VALUE) {
        if (params[1]->isList()) {
            auto vec = params[1]->toVector();
            vec.pop_back();
            return std::dynamic_pointer_cast<LambdaValue>(params[0])->apply(vec);
        }
        else {
            throw LispError("exprceted a list that can be applied on a procecure");
        }
    }
    else {
        throw LispError("expected a procedure that can be applied to arguments");
    }
}

ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_n_params(params, 1, "display");
    // if (params.empty()) {
    //     throw LispError("Need a param.");
    // }
    // if (params.size() > 1) {
    //     throw LispError("Params should be only 1.");
    // }
    auto param = params[0];
    if (param->getType() == ValueType::STRING_VALUE) {
        std::cout << std::dynamic_pointer_cast<StringValue>(params[0])->getValue() << std::endl;
    }
    else {
        std::cout << "\'" << param->toString();
    }
    return std::make_shared<NilValue>();
}

ValuePtr displayln(const std::vector<ValuePtr>& params, EvalEnv& env) {
    display(params, env);
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    check_n_params(params, 1, "error");
    auto param = env.eval(params[0]);
    if (param->getType() == ValueType::STRING_VALUE) {
        throw LispError(std::dynamic_pointer_cast<StringValue>(param)->getValue());
    }
    else {
        throw LispError("error procedure need string type param");
    }
}

ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    check_n_params(params, 1, "eval");
    return env.eval(params[0]);
}

ValuePtr _exit(const std::vector<ValuePtr>& params, EvalEnv& env) {
    check_n_params(params, 1, "exit");
    auto param = env.eval(params[0]);
    if (param->isNumber()) {
        std::exit(*param->asNumber());
    }
    else {
        throw LispError("params need to be a numeric value.");
    }
}

ValuePtr newline(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.empty()) {
        std::cout << std::endl;
        return std::make_shared<NilValue>();
    }
    else {
        throw LispError("The procedure should not have params.");
    }
}

ValuePtr atom(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_n_params(params, 1, "atom?");
    if (params[0]->getType() == ValueType::BOOLEAN_VALUE ||
        params[0]->getType() == ValueType::NUMERIC_VALUE ||
        params[0]->getType() == ValueType::STRING_VALUE ||
        params[0]->getType() == ValueType::SYMBOL_VALUE ||
        params[0]->getType() == ValueType::NIL_VALUE) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

ValuePtr boolean(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The boolean? procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->getType() == ValueType::BOOLEAN_VALUE) {
            return std::make_shared<BooleanValue>(true);
        }
        else {
            return std::make_shared<BooleanValue>(false);
        }
    }
    else {
        throw LispError("The boolean? procedure need only 1 params.");
    }
}

ValuePtr integer(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The integer? procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->isNumber() && static_cast<int>(*params[0]->asNumber()) == *params[0]->asNumber()) {
            return std::make_shared<BooleanValue>(true);
        }
        else {
            return std::make_shared<BooleanValue>(false);
        }
    }
    else {
        throw LispError("The integer? procedure need only 1 params.");
    }
}

ValuePtr list(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The list? procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->isList() || params[0]->isNil()) {
            return std::make_shared<BooleanValue>(true);
        }
        else {
            return std::make_shared<BooleanValue>(false);
        }
    }
    else {
        throw LispError("The list? procedure need only 1 params.");
    }
}

ValuePtr number(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The number? procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->isNumber()) {
            return std::make_shared<BooleanValue>(true);
        }
        else {
            return std::make_shared<BooleanValue>(false);
        }
    }
    else {
        throw LispError("The number? procedure need only 1 params.");
    }
}

ValuePtr null(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The null? procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->isNil()) {
            return std::make_shared<BooleanValue>(true);
        }
        else {
            return std::make_shared<BooleanValue>(false);
        }
    }
    else {
        throw LispError("The null? procedure need only 1 params.");
    }
}

ValuePtr _pair(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The pair? procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->getType() == ValueType::PAIR_VALUE) {
            return std::make_shared<BooleanValue>(true);
        }
        else {
            return std::make_shared<BooleanValue>(false);
        }
    }
    else {
        throw LispError("The pair? procedure need only 1 params.");
    }
}

ValuePtr isProcedure(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("THe procedure? procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE || params[0]->getType() == ValueType::LAMBDA_VALUE) {
            return std::make_shared<BooleanValue>(true);
        }
        else {
            return std::make_shared<BooleanValue>(false);
        }
    }
    else {
        throw LispError("The procedure? procedure need only 1 params.");
    }
}

ValuePtr isString(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The string? procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->getType() == ValueType::STRING_VALUE) {
            return std::make_shared<BooleanValue>(true);
        }
        else {
            return std::make_shared<BooleanValue>(false);
        }
    }
    else {
        throw LispError("The string? procedure need only 1 params.");
    }
}

ValuePtr isSymbol(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The symbol? procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->isSymbol()) {
            return std::make_shared<BooleanValue>(true);
        }
        else {
            return std::make_shared<BooleanValue>(false);
        }
    }
    else {
        throw LispError("The symbol? procedure need only 1 params.");
    }
}

namespace{
    ValuePtr makeList(std::vector<ValuePtr>& vec, int start) {
        if (vec.size() == 0) {
            return std::make_shared<NilValue>();
        }
        else if(vec.size() == 1) {
            return vec[0];
        }
        if (start == vec.size() - 2) {
            return std::make_shared<PairValue>(vec[start], vec[start + 1]);
        }
        else {
            return std::make_shared<PairValue>(vec[start], makeList(vec, start + 1));
        }
    }
}

ValuePtr append(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        return std::make_shared<NilValue>();
    }
    else if (params.size() == 1) {
        return params[0];
    }
    else {
        std::vector<ValuePtr> result{};
        for (int i = 0; i < static_cast<int>(params.size()) - 1; i++) {
            if (params[i]->isList()) {
                auto vec = params[i]->toVector();
                if (vec.back()->isNil()) 
                    vec.pop_back();
                std::ranges::copy(vec, std::back_inserter(result));
            }
            else if (!params[i]->isNil()) {
                std::cout << params[i]->toString() << std::endl;
                throw LispError("The append procedure need a list param.");
            }
        }
        if (params.back()->isNil()) {
            result.push_back(params.back());
        }
        std::ranges::copy(params.back()->toVector(), std::back_inserter(result));
        return makeList(result, 0);
    }
}

ValuePtr car(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The car procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->getType() == ValueType::PAIR_VALUE) {
            return std::dynamic_pointer_cast<PairValue>(params[0])->getLeft();
        }
        else {
            throw LispError("THe car procedure need pair value.");
        }
    }
    else {
        throw LispError("The car procedure need only 1 params.");
    }
}

ValuePtr cdr(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("THe cdr procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->getType() == ValueType::PAIR_VALUE) {
            return std::dynamic_pointer_cast<PairValue>(params[0])->getRight();
        }
        else {
            throw LispError("The cdr procedure need pair value.");
        }
    }
    else {
        throw LispError("The cdr procedure need only 1 params.");
    }
}

ValuePtr cons(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The cons procedure need 2 params, given 0.");
    }
    else if (params.size() == 1) {
        throw LispError("The cons procedure need 2 params, given 1.");
    }
    else if (params.size() == 2) {
        return std::make_shared<PairValue>(params[0], params[1]);
    }
    else {
        throw LispError("The cons procedure need only 2 params.");
    }
}

ValuePtr length(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The length proceduren need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->isList() || params[0]->isNil()) {
            if (params[0]->isNil()) {
                return std::make_shared<NumericValue>(0);
            }
            else {
                return std::make_shared<NumericValue>(static_cast<int>(params[0]->toVector().size()) - 1);
            }
        }
        else {
            throw LispError("The length procedure need list param.");
        }
    }
    else {
        throw LispError("The length procedure need only 1 params.");
    }
}

ValuePtr _list(const std::vector<ValuePtr>& params, EvalEnv&) {
    std::vector<ValuePtr> new_params{params};
    new_params.push_back(std::make_shared<NilValue>());
    return makeList(new_params, 0);
}

ValuePtr _map(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 0) {
        throw LispError("The map procedure need 2 params, given 0.");
    }
    else if (params.size() == 1) {
        throw LispError("The map procedure need 2 params, given 1.");
    }
    else if (params.size() == 2) {
        if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
            std::vector<ValuePtr>result{};
            if (params[1]->isList()) {
                auto vec = params[1]->toVector();
                vec.pop_back();
                std::ranges::transform(vec, std::back_inserter(result), [&env, &params](ValuePtr& ptr) -> ValuePtr {
                    return std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()({ptr}, env);
                });
                result.push_back(std::make_shared<NilValue>());
                return makeList(result, 0);
            }
            else if (params[1]->isNil()) {
                return std::make_shared<NilValue>();
            }
            else {
                throw LispError("The map procedure need list param as the second param.");
            }
        }
        else if (params[0]->getType() == ValueType::LAMBDA_VALUE) {
            std::vector<ValuePtr> result{};
            if (params[1]->isList()) {
                auto vec = params[1]->toVector();
                vec.pop_back();
                std::ranges::transform(vec, std::back_inserter(result), [&params](ValuePtr& ptr) -> ValuePtr {
                    return std::dynamic_pointer_cast<LambdaValue>(params[0])->apply({ptr});
                }) ;
                result.push_back(std::make_shared<NilValue>());
                return makeList(result, 0);
            }
            else if (params[1]->isNil()) {
                return std::make_shared<NilValue>();
            }
            else {
                throw LispError("The map procedure need list param as the second param.");
            }
        }
        else {
            throw LispError("The map procedure need procedure param as the first param.");
        }
    }
    else {
        throw LispError("The map procedure need only 2 params.");
    }
}

namespace{
    bool change_to_bool(const ValuePtr& ptr) {
        if (ptr->getType() == ValueType::BOOLEAN_VALUE && !std::dynamic_pointer_cast<BooleanValue>(ptr)->getValue()) {
            return false;
        }
        else {
            return true;
        }
    }
}

ValuePtr filter(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 0) {
        throw LispError("The filter procedure need 2 params, given 0.");
    }
    else if (params.size() == 1) {
        throw LispError("The filter procedure need 2 params, given 1.");
    }
    else if (params.size() == 2) {
        if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
            if (params[1]->isList()) {
                std::vector<ValuePtr> result{};
                auto vec = params[1]->toVector();
                vec.pop_back();
                std::ranges::copy_if(vec, std::back_inserter(result), [&env, &params](ValuePtr& ptr) -> bool {
                    return change_to_bool(std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()({ptr}, env));
                });
                result.push_back(std::make_shared<NilValue>());
                return makeList(result, 0);
            }
            else if (params[1]->isNil()) {
                return std::make_shared<NilValue>();
            }
            else {
                throw LispError("The filter procedure need list param as the second param.");
            }
        }
        else if (params[0]->getType() == ValueType::LAMBDA_VALUE) {
            if (params[1]->isList()) {
                std::vector<ValuePtr> result{};
                auto vec = params[1]->toVector();
                vec.pop_back();
                std::ranges::copy_if(vec, std::back_inserter(result), [&params](ValuePtr& ptr) -> bool {
                    return change_to_bool(std::dynamic_pointer_cast<LambdaValue>(params[0])->apply({ptr}));
                });
                result.push_back(std::make_shared<NilValue>());
                return makeList(result, 0);
            }
            else if (params[1]->isNil()) {
                return std::make_shared<NilValue>();
            }
            else {
                throw LispError("The filter procedure need list param as the second param.");
            }
        }
        else {
            throw LispError("The filter proceduren need procedure param as the first param.");
        }
    }
    else {
        throw LispError("The filter procedure need only 2 params.");
    }
}

ValuePtr _reduce(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 0) {
        throw LispError("The reduce procedure need 2 params, given 0.");
    }
    else if (params.size() == 1) {
        throw LispError("The reduce procedure need 2 params, given 1.");
    }
    else if (params.size() == 2) {
        if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
            if (params[1]->isList()) {
                if (std::dynamic_pointer_cast<NumericValue>(length({params[1]}, env))->getValue() == 1) {
                    return car({params[1]}, env);
                }
                else {
                    return std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()({car({params[1]}, env), _reduce({params[0], cdr({params[1]}, env)}, env)}, env);
                }
            }
            else if (params[1]->isNil()) {
                throw LispError("The reduce procedure cannot receive a null list as the second param.");
            }
            else {
                throw LispError("The reduce procedure need a list param as the second param.");
            }
        }
        else if (params[0]->getType() == ValueType::LAMBDA_VALUE) {
            if (params[1]->isList()) {
                if (std::dynamic_pointer_cast<NumericValue>(length({params[1]}, env))->getValue() == 1) {
                    return car({params[1]}, env);
                }
                else {
                    return std::dynamic_pointer_cast<LambdaValue>(params[0])->apply({car({params[1]}, env), _reduce({params[0], cdr({params[1]}, env)}, env)});
                }
            }
            else if (params[1]->isNil()) {
                throw LispError("The reduce procedure cannot receive a null list as the second param.");
            }
            else {
                throw LispError("The reduce procedure need a list param as the second param.");
            }
        }
        else {
            throw LispError("The reduce procedure need a procedure param as the first param.");
        }
    }
    else {
        throw LispError("The reduce procedure need only 2 params.");
    }
}

ValuePtr divide(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The / procedure need at least 1 param, given 0.");
    }
    else if (params.size() == 1) {
        if (params[0]->isNumber()) {
            if (*params[0]->asNumber() == 0) {
                throw LispError("Division by zero.");
            }
            return std::make_shared<NumericValue>(1.0 / *params[0]->asNumber());
        }
        else {
            throw LispError("Cannot divide a non-numeric value.");
        }
    }
    else if (params.size() == 2) {
        if (!params[0]->isNumber()) {
            throw LispError("Cannot divide a non-numeric value.");
        }
        else if (!params[1]->isNumber()) {
            throw LispError("Cannot use a non-numeric value to divide another value.");
        }
        if (*params[1]->asNumber() == 0) {
            throw LispError("Division by zero.");
        }
        return std::make_shared<NumericValue>(*params[0]->asNumber() / *params[1]->asNumber());
    }
    else {
        throw LispError("The / procedure need at most 2 params.");
    }
}

ValuePtr abs(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() == 0) {
        throw LispError("The abs procedure need 1 params.");
    }
    else if (params.size() == 1) {
        if (params[0]->isNumber()) {
            return std::make_shared<NumericValue>(std::abs(*params[0]->asNumber()));
        }
        else {
            throw LispError("The abs cannot receive a non-numeric value.");
        }
    }
    else {
        throw LispError("The abs procedure need only 1 params.");
    }
}

// namespace{
//     void check_n_params(const std::vector<ValuePtr>& params, int n, const std::string& procedure) {
//         if (params.size() != n) {
//             throw LispError("The " + procedure + " procedure need " + std::to_string(n) + " params, given " + std::to_string(int(params.size())) + ".");
//         }
//     }

//     void check_two_numbers(const std::vector<ValuePtr>& params, const std::string& procedure) {
//         check_n_params(params, 2, procedure);
//         if (!params[0]->isNumber() || !params[1]->isNumber()) {
//             throw LispError("The " + procedure + " procedure cannot receive a non-numeric value.");
//         }
//     }
    
//     void check_one_number(const std::vector<ValuePtr>& params, const std::string& procedure) {
//         check_n_params(params, 1, procedure);
//         if (!params[0]->isNumber()) {
//             throw LispError("The " + procedure + " procedure cannot receive a non-numeric value.");
//         }
//     }
// }

ValuePtr expt(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_two_numbers(params, "expt");
    if (*params[0]->asNumber() == 0 && *params[1]->asNumber() == 0) {
        throw LispError("Undefined! The base and the exponent cannot be zero in the same time.");
    }
    return std::make_shared<NumericValue>(std::pow(*params[0]->asNumber(), *params[1]->asNumber()));
}

ValuePtr quotient(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_two_numbers(params, "quotient");
    double x = *params[0]->asNumber();
    double y = *params[1]->asNumber();
    if (y == 0) {
        throw LispError("Divide by zero.");
    }
    double result = x / y;
    if (static_cast<int>(result) == result) 
        return std::make_shared<NumericValue>(result);
    else 
        return std::make_shared<NumericValue>(static_cast<int>(result));
}

ValuePtr modulo(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_two_numbers(params, "modulo");
    double x = *params[0]->asNumber();
    double y = *params[1]->asNumber();
    if (static_cast<int>(x) != x || static_cast<int>(y) != y) {
        throw LispError("The modulo procedure should receive integers, given not-integers.");
    }
    if (y == 0) {
        throw LispError("Divide by zero.");
    }
    int a = static_cast<int>(x);
    int b = static_cast<int>(y);
    int result = a % b;
    if (result * b < 0) 
        result += b;
    return std::make_shared<NumericValue>(result);
}

ValuePtr remainder(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_two_numbers(params, "remainder");
    double x = *params[0]->asNumber();
    double y = *params[1]->asNumber();
    if (static_cast<int>(x) != x || static_cast<int>(y) != y) {
        throw LispError("The remainder procedure should receive integers, given not-integers.");
    }
    if (y == 0) {
        throw LispError("Divide by zero.");
    }
    int result = x - y * static_cast<int>(x / y);
    return std::make_shared<NumericValue>(result);
}

namespace{
    bool isEqual(ValuePtr p1, ValuePtr p2, EvalEnv& env) {
        if (p1->getType() != p2->getType()) {
            return false;
        }
        else {
            if (p1->getType() == ValueType::BOOLEAN_VALUE) {
                return std::dynamic_pointer_cast<BooleanValue>(p1)->getValue() == std::dynamic_pointer_cast<BooleanValue>(p2)->getValue();
            }
            else if (p1->getType() == ValueType::NUMERIC_VALUE) {
                return std::dynamic_pointer_cast<NumericValue>(p1)->getValue() == std::dynamic_pointer_cast<NumericValue>(p2)->getValue();
            }
            else if (p1->getType() == ValueType::STRING_VALUE) {
                return std::dynamic_pointer_cast<StringValue>(p1)->getValue() == std::dynamic_pointer_cast<StringValue>(p2)->getValue();
            }
            else if (p1->getType() == ValueType::NIL_VALUE) {
                return true;
            }
            else if (p1->getType() == ValueType::SYMBOL_VALUE) {
                return std::dynamic_pointer_cast<SymbolValue>(p1)->getName() == std::dynamic_pointer_cast<SymbolValue>(p2)->getName();
            }
            else if (p1->getType() == ValueType::PAIR_VALUE) {
                return isEqual(car({p1}, env), car({p2}, env), env) && isEqual(cdr({p1}, env), cdr({p2}, env), env);
            }
            else if (p1->getType() == ValueType::BUILTIN_PROC_VALUE) {
                return std::dynamic_pointer_cast<BuiltinProcValue>(p1)->getFunction() == std::dynamic_pointer_cast<BuiltinProcValue>(p2)->getFunction();
            }
            else if (p1->getType() == ValueType::LAMBDA_VALUE) {
                return p1.get() == p2.get();
            }
            else {
                throw LispError("Unimplement in equal?.");
            }
        }
    }
}

ValuePtr _eq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    check_n_params(params, 2, "eq?");
    if (params[0]->isSelfEvaluating() && params[1]->isSelfEvaluating()) {
        return std::make_shared<BooleanValue>(isEqual(params[0], params[1], env));
    }
    else if (params[0]->isSymbol() && params[1]->isSymbol()) {
        std::cout << "Both symbol." << std::endl;
        return std::make_shared<BooleanValue>(isEqual(params[0], params[1], env));
    }
    else if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE && params[1]->getType() == ValueType::BUILTIN_PROC_VALUE) {
        return std::make_shared<BooleanValue>(isEqual(params[0], params[1], env));
    }
    else if (params[0]->getType() == ValueType::NIL_VALUE && params[1]->getType() == ValueType::NIL_VALUE) {
        return std::make_shared<BooleanValue>(true);
    }
    if (params[0].get() == params[1].get()) {
        return std::make_shared<BooleanValue>(true);
    }
    else {
        return std::make_shared<BooleanValue>(false);
    }
}

ValuePtr _equal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    check_n_params(params, 2, "equal?");
    return std::make_shared<BooleanValue>(isEqual(params[0], params[1], env));
}

ValuePtr _not(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_n_params(params, 1, "not");
    return std::make_shared<BooleanValue>(!change_to_bool(params[0]));
}

ValuePtr __equal(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_two_numbers(params, "=");
    return std::make_shared<BooleanValue>(*params[0]->asNumber() == *params[1]->asNumber());
}

ValuePtr less(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_two_numbers(params, "<");
    return std::make_shared<BooleanValue>(*params[0]->asNumber() < *params[1]->asNumber());
}

ValuePtr larger(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_two_numbers(params, ">");
    return std::make_shared<BooleanValue>(*params[0]->asNumber() > *params[1]->asNumber());
}

ValuePtr less_or_equal(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_two_numbers(params, "<=");
    return std::make_shared<BooleanValue>(*params[0]->asNumber() <= *params[1]->asNumber());
}

ValuePtr larger_or_equal(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_two_numbers(params, ">=");
    return std::make_shared<BooleanValue>(*params[0]->asNumber() >= *params[1]->asNumber());
}

ValuePtr even(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_one_number(params, "even?");
    if (static_cast<int>(*params[0]->asNumber()) != *params[0]->asNumber()) {
        throw LispError("The procedure even? cannot receive a non-integer value.");
    }
    return std::make_shared<BooleanValue>(static_cast<int>(*params[0]->asNumber()) % 2 == 0);
}

ValuePtr odd(const std::vector<ValuePtr>& params, EvalEnv& env) {
    check_one_number(params, "odd?");
    if (static_cast<int>(*params[0]->asNumber()) != *params[0]->asNumber()) {
        throw LispError("The procedure odd? cannot receive a non-integer value.");
    }
    return std::make_shared<BooleanValue>(static_cast<int>(*params[0]->asNumber()) % 2 != 0);
}

ValuePtr zero(const std::vector<ValuePtr>& params, EvalEnv&) {
    check_one_number(params, "zero?");
    return std::make_shared<BooleanValue>(*params[0]->asNumber() == 0);
}

std::unordered_map<std::string, BuiltinFuncType*> innerSymbolTable{
    {"+", &add},
    {"*", &multiply}, 
    {"-", &reduce}, 
    {"/", &divide}, 
    {"abs", &abs}, 
    {"expt", &expt}, 
    {"quotient", &quotient}, 
    {"modulo", &modulo}, 
    {"remainder", &remainder}, 
    {"eq?", &_eq}, 
    {"equal?", &_equal}, 
    {"not", &_not}, 
    {"=", &__equal}, 
    {"<", &less}, 
    {">", &larger}, 
    {"<=", &less_or_equal}, 
    {">=", &larger_or_equal}, 
    {"even?", &even}, 
    {"odd?", &odd}, 
    {"zero?", &zero}, 
    {"apply", &apply}, 
    {"display", &display}, 
    {"displayln", &displayln}, 
    {"error", &error}, 
    {"exit", &_exit}, 
    {"newline", &newline}, 
    {"print", &print}, 
    {"eval", &eval}, 
    {"atom?", &atom}, 
    {"boolean?", &boolean}, 
    {"integer?", &integer}, 
    {"list?", &list}, 
    {"number?", &number}, 
    {"null?", &null}, 
    {"pair?", &_pair}, 
    {"procedure?", &isProcedure}, 
    {"string?", &isString}, 
    {"symbol?", &isSymbol}, 
    {"append", &append}, 
    {"car", &car}, 
    {"cdr", &cdr}, 
    {"cons", &cons}, 
    {"length", &length}, 
    {"len", &length}, 
    {"list", &_list}, 
    {"map", &_map}, 
    {"filter", &filter}, 
    {"reduce", &_reduce}
};
