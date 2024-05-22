/* ************************************************************************
> File Name:     pair_parser.cpp
> Author:        林毅波
> Created Time:  2024/5/22 11:55:07
> Description:   
 ************************************************************************/

#include "../include/pair_parser.h"

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
    void backtracking(std::vector<ValuePtr>& result, ValuePtr ptr) {
        if(ptr->isList()) { // 如果值是列表
            auto p = std::dynamic_pointer_cast<PairValue>(ptr);
            backtracking(result, p->getLeft()); // 递归处理左侧部分
            backtracking(result, p->getRight()); // 递归处理右侧部分
        }
        else {
            result.push_back(ptr); // 如果值不是泪飙，将其添加到结果向量中
        }
    }
}

std::vector<ValuePtr> PairParser::parse() const {
    std::vector<ValuePtr> vec{}; // 创建空的值指针向量
    ValuePtr ptr(std::const_pointer_cast<Value>(pairPtr)); // 创建 PairValue 对象的指针
    backtracking(vec, ptr); // 调用回溯函数进行转换
    return vec; // 返回转换后的值指针向量
};
