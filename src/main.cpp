#include <iostream>
#include <string>

#include <stdexcept>
#include "model.h"
#include "reader.h"
#include "../src/rjsj_test.hpp"

struct TestCtx {
    std::shared_ptr<EvalEnv> env = EvalEnv::createEvalEnv();
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result = env->eval(std::move(value));
        return result->toString();
    }
};

int main(int argc, char** argv) {
    // RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    std::shared_ptr<EvalEnv> env = EvalEnv::createEvalEnv();
    if (argc == 1) {
        while (true) {
            try {
                std::cout << ">>> " ;
                Repl().carryOut(env);
            }
            catch (std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }
    else if (argc == 2) {
        try{
            std::string fileName = argv[1];
            File(argv[1]).carryOut(env);
        }
        catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    else {
        std::cout << "The args are more than needed." << std::endl;
    }
}
