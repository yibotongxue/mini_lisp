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
/**
 *这是负责解析lisp语法的类,也就是读取器
 *
 *
 */
class Parser{
private:
    std::deque<TokenPtr> tokens;    // 一系列词法标记
public:
    /**
     * Paraser类的构造函数，从一系列词法标记构造
     * @param tokens: 一系列词法标记
     */

    Parser(std::deque<TokenPtr> tokens);
    
    /*
     * 解析函数，解析对象中的词法标记
     */
    std::shared_ptr<Value> parse();
};

