/* ************************************************************************
> File Name:     parser.h
> Author:        林毅波
> Created Time:  2024/5/12 10:10:56
> Description:  lisp的语法分析器 
 ************************************************************************/
#include "../include/token.h"
#include "../include/tokenizer.h"
#include "../include/value.h"
#include <queue>
/*
 *
 * */
class Paraser{
private:
    std::deque<TokenPtr> tokens;
public:
    Paraser(std::deque<TokenPtr> tokens);
    std::share_ptr<Value> parse();
}

