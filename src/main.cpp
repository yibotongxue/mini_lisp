#include <iostream>
#include "../include/value.h"

using ValuePtr = std::shared_ptr<Value>; // 把这个添加到 value.h，可以减少许多重复的代码。
int main() {
    ValuePtr a = std::make_shared<NumericValue>(42);
    ValuePtr b = std::make_shared<BooleanValue>(false);
    ValuePtr c = std::make_shared<SymbolValue>("eq?");
    ValuePtr d = std::make_shared<StringValue>("Hello\"");
    ValuePtr e = std::make_shared<NilValue>();
    ValuePtr f = std::make_shared<PairValue>(
        c,
        std::make_shared<PairValue>(
            a,
            std::make_shared<PairValue>(d, e)
        )
    );
    std::cout << a->toString() << '\n'
              << b->toString() << '\n'
              << c->toString() << '\n'
              << d->toString() << '\n'
              << e->toString() << '\n'
              << f->toString() << std::endl;
}