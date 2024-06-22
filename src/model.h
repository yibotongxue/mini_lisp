#ifndef __MODEL_H__
#define __MODEL_H__

#include "parser.h"
#include "tokenizer.h"
#include "eval_env.h"

/**
 * @brief 抽象基类 Model，定义模型的基本行为。
 * 
 * @details 派生类需要实现纯虚函数 readTokens() 和 carryOut()。
 */
class Model {
protected:
    virtual ~Model() = default;
    /**
     * @brief 纯虚函数，从输入中读取 Token 并构建 Token 队列。
     * 
     * @return Token 队列
     */
    virtual std::deque<TokenPtr> readTokens() = 0;
public:
    /**
     * @brief 纯虚函数，执行模型中的操作。
     * 
     * @param env 求值环境
     */
    virtual void carryOut(std::shared_ptr<EvalEnv>& env) = 0;
};

/**
 * @brief 派生类 Repl，实现 REPL（Read-Eval-Print Loop）的模型。
 * 
 * @details 重写了基类的 readTokens() 方法，处理标准输入并构建 Token 队列。
 */
class Repl : public Model {
private:
    /**
     * @brief 从标准输入读取 Token 并构建 Token 队列。
     * 
     * @return Token 队列
     * @throws SyntaxError 如果括号不匹配
     */
    std::deque<TokenPtr> readTokens() override;
public:
    /**
     * @brief 执行 REPL 中的表达式。
     * 
     * @param env 求值环境
     */
    void carryOut(std::shared_ptr<EvalEnv>& env) override;   
};

class File : public Model {
private:
    std::string fileName;
    /**
     * @brief 从文件中读取 Token。
     * 
     * @return 存储 Token 的 deque
     * @throws FileError 如果打开或读取文件失败
     */
    std::deque<TokenPtr> readTokens() override;
public:
    /**
     * @brief 构造函数，指定文件名。
     * 
     * @param fileName 文件名
     */
    File(const std::string& fileName) : fileName{fileName} {}
    
    /**
     * @brief 执行文件中的表达式。
     * 
     * @param env 求值环境
     */
    void carryOut(std::shared_ptr<EvalEnv>& env) override;  
};

#endif