#ifndef __READER_H__
#define __READER_H__

#include "parser.h"
#include "eval_env.h"
#include <iostream>

/**
 * @brief Reader 类负责输入解析和值的缓存。
 * 
 * @details 它从标准输入读取输入，处理缩进、处理括号匹配，并将输入行解析为 Token。
 *          解析得到的 Token 存储在缓存中，以备后续使用。
 */
class Reader {
private:
    std::deque<ValuePtr> cache;

    /**
     * @brief 私有构造函数，用于实现单例模式。
     */    
    Reader() : cache{} {}
public:
    /**
     * @brief 获取 Reader 类的单例实例。
     * 
     * @return 静态的 Reader 对象引用
     */
    static Reader& getInstance();

    /**
     * @brief 检查缓存是否为空。
     * 
     * @return 如果缓存为空，返回 true；否则返回 false
     */
    bool empty() const;

    /**
     * @brief 从标准输入读取输入并解析为 Token。
     * 
     * @details 处理多行输入、缩进和括号匹配。
     */
    void readIn();

    /**
     * @brief 获取缓存中的下一个值。
     * 
     * @return 指向缓存中的值的指针
     */
    ValuePtr output();
};

#endif