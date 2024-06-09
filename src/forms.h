#ifndef __FORMS_H__
#define __FORMS_H__

#include <unordered_map>
#include "./value.h"
#include "./eval_env.h"

using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

extern std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS;

#endif