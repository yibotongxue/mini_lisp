/* ************************************************************************
> File Name:     pair_parser.h
> Author:        林毅波
> Created Time:  2024/5/22 11:54:49
> Description:   
 ************************************************************************/
#ifndef __PAIR_PARSER_H__
#define __PAIR_PARSER_H__


# include "../include/value.h"
# include "../include/builtins.h"
# include <unordered_set>

class PairParser {
private:
    std::unordered_set<std::string> needTogetherSet;

public:
    PairParser();

    std::vector<ValuePtr> parse(ValuePtr ptr);
};

#endif