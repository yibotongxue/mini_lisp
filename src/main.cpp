#include <iostream>
#include <string>

#include "../include/tokenizer.h"
#include "../include/parser.h"

int main() {
    while (true) {
        try {
            std::cout << ">>> " ;
            std::string line;
            std::getline(std::cin, line);
            if (std::cin.eof()) {
                std::exit(0);
            }
            auto tokens = Tokenizer::tokenize(line);
            Parser parser(std::move(tokens));
            auto value = parser.parse();
            std::cout << value->toString() << std::endl;
            // for (auto& token : tokens) {
            //     std::cout << *token << std::endl;
            // }
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
