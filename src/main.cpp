#include <iostream>
#include <new>
#include <stdexcept>
#include <string>

#include "../src/rjsj_test.hpp"
#include "model.h"
#include "reader.h"

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
    RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    std::shared_ptr<EvalEnv> env = EvalEnv::createEvalEnv();
    if (argc == 1) {
        while (true) {
            try {
                std::cout << ">>> ";
                Repl().carryOut(env);
            } catch (std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            } catch (std::bad_alloc& b) {
                std::cerr << "Error: Unimplement which may cause bad_alloc."
                          << std::endl;
            } catch (...) {
                std::cerr << "Error: Unimplement." << std::endl;
            }
        }
    } else if (argc == 2) {
        int previousLineNumber = 0;
        File file;
        try {
            std::string fileName = argv[1];
            file = File(argv[1]);
            while (true) {
                file.carryOut(env);
                previousLineNumber = file.getLineNumber();
            }
        } catch (std::runtime_error& e) {
            std::cerr << "Error in line " << std::to_string(previousLineNumber)
                      << " to line " << std::to_string(file.getLineNumber())
                      << ": " << e.what() << std::endl;
        } catch (std::bad_alloc& b) {
            std::cerr << "Error in line " << std::to_string(previousLineNumber)
                      << " to line " << std::to_string(file.getLineNumber())
                      << ": Unimplement." << std::endl;
        } catch (...) {
            std::cerr << "Error in line " << std::to_string(previousLineNumber)
                      << " to line " << std::to_string(file.getLineNumber())
                      << ": Unimplement." << std::endl;
        }
    } else {
        std::cout << "The args are more than needed." << std::endl;
    }
}
