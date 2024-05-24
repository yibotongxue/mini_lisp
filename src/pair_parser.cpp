/* ************************************************************************
> File Name:     pair_parser.cpp
> Author:        林毅波
> Created Time:  2024/5/22 11:55:07
> Description:   
 ************************************************************************/

#include "../include/pair_parser.h"
#include <iostream>

PairParser::PairParser() : innerTable{} {
    innerTable.insert("+");
    innerTable.insert("print");
}

void PairParser::add(const std::string& s) {
    innerTable.insert(s);
}

const std::unordered_set<std::string> PairParser::getSet() const {
    return innerTable;
}

namespace{
    /**
    * @brief 回溯函数，将 PairValue 对象转换为值指针向量
    * 
    * @param result 用于存储转换后的值指针向量
    * @param ptr 要转换的 PairValue 对象的指针
    * 
    * @note 如果给定的值是一个对子（PairValue 对象），则递归地遍历其左右部分。
    *       如果给定的值不是列表，则将其添加到结果向量中。
    */
    std::vector<ValuePtr> backtracking(PairParser& parser, ValuePtr ptr) {
        std::vector<ValuePtr> result{};
        if(ptr->isList()) { // 如果值是列表
            auto p = std::dynamic_pointer_cast<PairValue>(ptr);
            if (p->getLeft()->isSymbol()) {
                auto symbolPtr = std::dynamic_pointer_cast<SymbolValue>(p->getLeft());
                auto set = parser.getSet();
                if (set.find(symbolPtr->getName()) != set.end()) {
                    result.push_back(ptr);
                    return result;
                }
            }
            std::ranges::copy(backtracking(parser, p->getLeft()), std::back_inserter(result)); // 递归处理左侧部分
            std::ranges::copy(backtracking(parser, p->getRight()), std::back_inserter(result)); // 递归处理右侧部分
        }
        else {
            result.push_back(ptr); // 如果值不是列表，将其添加到结果向量中
        }
        return result;
    }
}

std::vector<ValuePtr> PairParser::parse(ValuePtr ptr) {
    ValuePtr notConstPtr(std::const_pointer_cast<Value>(ptr)); // 创建 PairValue 对象的指针
    if (notConstPtr->isList()) {
        auto p = std::dynamic_pointer_cast<PairValue>(notConstPtr);
        std::vector<ValuePtr> result{};
        std::ranges::copy(backtracking(*this, p->getLeft()), std::back_inserter(result));
        std::ranges::copy(backtracking(*this, p->getRight()), std::back_inserter(result));
        return result;
    }
    else if (notConstPtr->isNil()) {
        return {};
    }
    else {
        return { notConstPtr };
    }
};
