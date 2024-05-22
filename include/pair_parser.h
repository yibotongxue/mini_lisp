/* ************************************************************************
> File Name:     pair_parser.h
> Author:        林毅波
> Created Time:  2024/5/22 11:54:49
> Description:   
 ************************************************************************/

# include "../include/value.h"

class PairParser {
private:
    ValuePtr pairPtr;
public:
    PairParser(ValuePtr ptr) : pairPtr{ptr} {}

    std::vector<ValuePtr> parse() const;
};
