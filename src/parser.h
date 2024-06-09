/* ************************************************************************
> File Name:     parser.h
> Author:        林毅波
> Created Time:  2024/5/12 10:10:56
> Description:   lisp的语法分析器 
 ************************************************************************/
#ifndef __PARSER_H
#define __PARSER_H


#include "token.h"
#include "tokenizer.h"
#include "value.h"
#include <queue>

/**
 * @brief 这是负责解析lisp语法的类,也就是读取器
 * tokens作为解析的对象，成为Parser对象的私有变量，控制访问
 * Parser类有构造函数，可以从 std::deque<TokenPtr> tokens 构造
 * 函数 parse 负责解析对象，返回 Value 对象的共享指针
 * @note 构造的时候，由于 TokenPtr 对象不允许复制，需要移动构造
 */
class Parser{
private:
    std::deque<TokenPtr> tokens;    // 一系列词法标记

public:
    Parser();

    /**
     * Paraser类的构造函数，从一系列词法标记构造
     * @param tokens: 一系列词法标记
     */
    Parser(std::deque<TokenPtr> tokens);
    
    /*
     * 解析函数，解析对象中的词法标记
     */
    ValuePtr parse();

    /*
     * 解析 S-表达式
     * @return 如果词法标记为空，返回空表，否则返回对子
     */
    ValuePtr parseTails();

    bool empty() const;
};

#endif