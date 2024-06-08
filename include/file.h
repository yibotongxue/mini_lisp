#ifndef __FILE_H__
#define __FILE_H__

#include "./tokenizer.h"
#include "./parser.h"
#include "./eval_env.h"

/**
 * @brief File类表示文件操作的工具类。
 * 
 * 该类用于从指定的文件中读取Token，并将其存储在一个deque中。它还提供了执行文件中表达式的功能，
 * 将Token解析为Value，并在给定的求值环境中进行求值。
 */
class File {
private:
    std::string fileName; // 文件名

    /**
     * @brief 构造函数，创建File对象。
     * 
     * @param name 文件名
     */
    File(const std::string& fileName) : fileName{fileName} {}

    /**
     * @brief 执行文件中的表达式。
     * 
     * @param env 求值环境
     */
    void carryOut(std::shared_ptr<EvalEnv>& env);

    /**
     * @brief 从文件中读取Token。
     * 
     * @return 存储Token的deque
     * @throws FileError 如果打开或读取文件失败
     */
    std::deque<TokenPtr> readTokens(); 

public:
    /**
     * @brief 执行指定文件中的表达式。
     * 
     * @param fileName 文件名
     * @param env 求值环境
     */
    static void carryOut(const std::string& fileName, std::shared_ptr<EvalEnv>& env);
};

#endif