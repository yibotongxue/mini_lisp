/* ************************************************************************
> File Name:     pair_parser.h
> Author:        林毅波
> Created Time:  2024/5/22 11:54:49
> Description:   
 ************************************************************************/

# include "../include/value.h"
# include <unordered_set>
# include <algorithm>
# include <iterator>

class PairParser {
private:
    std::unordered_set<std::string> innerTable;
public:
    PairParser();

    void add(const std::string& s);

    const std::unordered_set<std::string> getSet() const;

    std::vector<ValuePtr> parse(ValuePtr ptr);
};