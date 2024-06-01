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
        std::cout << param->toString() << std::endl;
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

ValuePtr larger(const std::vector<ValuePtr>& params, EvalEnv&) {
    if (params.size() < 2) {
        throw LispError("Less params than needed.");
    }
    else {
        bool result = true;
        if (!params[0]->isNumber()) {
            throw LispError("Cannot compare a non-numeric value.");
        }
        for (int i = 1; i < params.size(); i++) {
            if (params[i]->isNumber()) {
                result = result && *params[i]->asNumber() < *params[i - 1]->asNumber();
            }
            else {
                throw LispError("Cannot compare a non-numeric value.");
            }
        }
        return std::make_shared<BooleanValue>(result);
    }
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
    if (params.size() == 1) {
        throw LispError("A param is needed.");
    }
    if (params.size() > 2) {
        throw LispError("The params are more than needed.");
    }
    if (params[0]->getType() == ValueType::BUILTIN_PROC_VALUE) {
        if (params[1]->isList() || params[1]->isNil()) {
            auto vec = params[1]->toVector();
            std::vector<ValuePtr> args;
            for (int i = 0; i < vec.size(); i++) {
                if (vec[i]->isList()) {
                    args.push_back(env.eval(std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft()));
                }
                else if (!vec[i]->isNil()) {
                    args.push_back(env.eval(vec[i]));
                }
            }
            return std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()(args, env);
        }
        else {
            throw LispError("expected a list that can be applied on a procedure");
        }
    }
    else if (params[0]->getType() == ValueType::LAMBDA_VALUE) {
        if (params[1]->isList()) {
            auto vec = params[1]->toVector();
            std::vector<ValuePtr> args;
            for (int i = 0; i < vec.size(); i++) {
                if (vec[i]->isList()) {
                    args.push_back(env.eval(std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft()));
                }
                else if (!vec[i]->isNil()) {
                    args.push_back(env.eval(vec[i]));
                }
            }
            return std::dynamic_pointer_cast<LambdaValue>(params[0])->apply(args);
        }
        else {
            throw LispError("exprceted a list that can be applied on a procecure");
        }
    }
    else {
        throw LispError("expected a procedure that can be applied to arguments");
    }
}

ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) {
        throw LispError("Need a param.");
    }
    if (params.size() > 1) {
        throw LispError("Params should be only 1.");
    }
    auto param = env.eval(params[0]);
    if (param->getType() == ValueType::STRING_VALUE) {
        std::cout << std::dynamic_pointer_cast<StringValue>(params[0])->getValue() << std::endl;
    }
    else {
        std::cout << "\'" << env.eval(param)->toString();
    }
    return std::make_shared<NilValue>();
}

ValuePtr displayln(const std::vector<ValuePtr>& params, EvalEnv& env) {
    display(params, env);
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 0) {
        throw LispError("error procedure need 1 param.");
    }
    else if (params.size() == 1) {
        auto param = env.eval(params[0]);
        if (param->getType() == ValueType::STRING_VALUE) {
            throw LispError(std::dynamic_pointer_cast<StringValue>(param)->getValue());
        }
        else {
            throw LispError("error procedure need string type param");
        }
    }
    else {
        throw LispError("More params than needed in error procedure.");
    }
}

ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 0) {
        throw LispError("procedure eval need 1 param, given 0.");
    }
    else if (params.size() == 1) {
        return env.eval(params[0]);
    }
    else {
        throw LispError("procedure eval cannot receive more than 1 params.");
    }
}

ValuePtr _exit(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 0) {
        std::exit(0);
    }
    else if (params.size() == 1) {
        auto param = env.eval(params[0]);
        if (param->isNumber()) {
            std::exit(*param->asNumber());
        }
        else {
            throw LispError("params need to be a numeric value.");
        }
    }
    else {
        throw LispError("params are more than needed.");
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
    if (params.size() == 0) {
        throw LispError("The atom? procedure need 1 params.");
    }
    else if (params.size() == 1) {
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
    else {
        throw LispError("The atom? procedure need only 1 params.");
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
        if (params[0]->getType() == ValueType::PAIR_VALUE && !params[0]->isList()) {
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
        for (int i = 0; i < params.size() - 1; i++) {
            if (params[i]->isList() ||params[i]->isNil()) {
               auto vec = params[i]->toVector();
               if (vec.size() > 0) {
                   result.push_back(vec[0]);
               }
               for (int j = 1; j < vec.size(); j++) {
                   if (vec[j]->getType() == ValueType::PAIR_VALUE) {
                       result.push_back(std::dynamic_pointer_cast<PairValue>(vec[j])->getLeft());
                   }
               }
            }
            else {
                throw LispError("The append procedure need a list param.");
            }
        }
        if (params.back()->isNil()) {
            result.push_back(params.back());
        }
        auto vec = params.back()->toVector();
        for (auto& ptr : vec) {
            if (ptr->getType() == ValueType::PAIR_VALUE) {
                result.push_back(std::dynamic_pointer_cast<PairValue>(ptr)->getLeft());
            }
            else {
                result.push_back(ptr);
            }
        }
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
                return std::make_shared<NumericValue>(params[0]->toVector().size() - 1);
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
                if (vec.size() > 0)
                    result.push_back(std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()({vec[0]}, env));
                for (int i = 1; i < vec.size() - 1; i++) {
                    if (vec[i]->getType() == ValueType::PAIR_VALUE) {
                        result.push_back(std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()({std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft()}, env));
                    }
                }
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
                if (vec.size() > 0)
                    result.push_back(std::dynamic_pointer_cast<LambdaValue>(params[0])->apply({vec[0]}));
                for (int i = 1; i < vec.size() - 1; i++) {
                    if (vec[i]->getType() == ValueType::PAIR_VALUE) {
                        result.push_back(std::dynamic_pointer_cast<LambdaValue>(params[0])->apply({std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft()}));
                    }
                }
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
                if (vec.size() > 0) {
                    if (change_to_bool(std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()({vec[0]}, env))) {
                        result.push_back(vec[0]);
                    }
                }
                for (int i = 1; i < vec.size() - 1; i++) {
                    if (vec[i]->getType() == ValueType::PAIR_VALUE) {
                        if (change_to_bool(std::dynamic_pointer_cast<BuiltinProcValue>(params[0])->getFunction()({std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft()}, env))) {
                            result.push_back(std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft());
                        }
                    }
                }
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
                if (vec.size() > 0) {
                    if (change_to_bool(std::dynamic_pointer_cast<LambdaValue>(params[0])->apply({vec[0]}))) {
                        result.push_back(vec[0]);
                    }
                }
                for (int i = 1; i < vec.size() - 1; i++) {
                    if (vec[i]->getType() == ValueType::PAIR_VALUE) {
                        if (change_to_bool(std::dynamic_pointer_cast<LambdaValue>(params[0])->apply({std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft()}))) {
                            result.push_back(std::dynamic_pointer_cast<PairValue>(vec[i])->getLeft());
                        }
                    }
                }
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

std::unordered_map<std::string, BuiltinFuncType*> innerSymbolTable{
    {"+", &add},
    {"print", &print}, 
    {"*", &multiply}, 
    {">", &larger}, 
    {"-", &reduce}, 
    {"apply", &apply}, 
    {"display", &display}, 
    {"displayln", &displayln}, 
    {"error", &error}, 
    {"exit", &_exit}, 
    {"newline", &newline}, 
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
    {"list", &_list}, 
    {"map", &_map}, 
    {"filter", &filter}, 
    {"reduce", &_reduce}
};
