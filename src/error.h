#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>

// 输入的语法错误，导致不能正常解析
class SyntaxError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

// 解释过程出现的错误
class LispError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

// 解释文本时遇到的错误
class FileError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

#endif
