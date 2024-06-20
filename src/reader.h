#ifndef __READER_H__
#define __READER_H__

#include "parser.h"
#include "eval_env.h"
#include <iostream>

class Reader {
private:
    std::deque<ValuePtr> cache;
    
    Reader() : cache{} {}
public:
    static Reader& getInstance();

    bool empty() const;

    void readIn();

    ValuePtr output();
};

#endif