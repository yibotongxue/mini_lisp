#include "tokenizer.h"

#include <cctype>
#include <set>
#include <stdexcept>
#include <algorithm>

#include "error.h"

const std::set<char> TOKEN_END{'(', ')', '\'', '`', ',', '"'};

TokenPtr Tokenizer::nextToken(int& pos) {
    while (pos < input.size()) {
        auto c = input[pos];
        if (c == ';') {
            while (pos < input.size() && input[pos] != '\n') {
                pos++;
            }
        } else if (std::isspace(c)) {
            pos++;
        } else if (auto token = Token::fromChar(c)) {
            pos++;
            return token;
        } else if (c == '#') {
            if (auto result = BooleanLiteralToken::fromChar(input[pos + 1])) {
                pos += 2;
                return result;
            } else {
                throw SyntaxError("Unexpected character after #");
            }
        } else if (c == '"') {
            std::string string;
            pos++;
            while (pos < input.size()) {
                if (input[pos] == '"') {
                    pos++;
                    return std::make_unique<StringLiteralToken>(string);
                } else if (input[pos] == '\\') {
                    if (pos + 1 >= input.size()) {
                        throw SyntaxError("Unexpected end of string literal");
                    }
                    auto next = input[pos + 1];
                    if (next == 'n') {
                        string += '\n';
                    } else {
                        string += next;
                    }
                    pos += 2;
                } else {
                    string += input[pos];
                    pos++;
                }
            }
            throw SyntaxError("Unexpected end of string literal");
        } else {
            int start = pos;
            do {
                pos++;
            } while (pos < input.size() && !std::isspace(input[pos]) &&
                     TOKEN_END.find(input[pos]) == TOKEN_END.end());
            auto text = input.substr(start, pos - start);
            if (text == ".") {
                return Token::dot();
            }
            if (text[0] == '.') {
                bool allIsDigit = true;
                for (int i = 1; i < text.size(); i++) {
                    if (!std::isdigit(text[i])) {
                        allIsDigit = false;
                        break;
                    }
                }
                if (allIsDigit) {
                    try {
                        return std::make_unique<NumericLiteralToken>(std::stod(text));
                    } catch (std::invalid_argument& e) {

                    }
                }
            }
            else if (std::isdigit(text[0]) || text[0] == '+' || text[0] == '-') {
                int nextNotDigitPos = 1;
                while (nextNotDigitPos < text.size()) {
                    if (!std::isdigit(text[nextNotDigitPos])) {
                        break;
                    }
                    nextNotDigitPos++;
                }
                if (nextNotDigitPos >= text.size()) {
                    try {
                        return std::make_unique<NumericLiteralToken>(std::stod(text));
                    } catch (std::invalid_argument& e) {

                    }
                }
                else if (text[nextNotDigitPos] == '/') {
                    char* new_text = new char[nextNotDigitPos + 1];
                    for (int i = 0; i < nextNotDigitPos; i++) {
                        new_text[i] = text[i];
                    }
                    new_text[nextNotDigitPos] = '\0';
                    int numerator = std::atoi(new_text);
                    delete []new_text;
                    new_text = new char[static_cast<int>(text.size()) - nextNotDigitPos];
                    int len = 0;
                    for (int i = 0; i < static_cast<int>(text.size()) - nextNotDigitPos - 1; i++) {
                        if (text[nextNotDigitPos + i + 1] == ' ' || text[i + nextNotDigitPos + 1] == '\n') {
                            break;
                        }
                        len++;
                        if (!std::isdigit(text[i + nextNotDigitPos + 1])) {
                            throw SyntaxError("Unimplement.");
                        }
                        new_text[i] = text[i + nextNotDigitPos + 1];
                    }
                    new_text[len] = '\0';
                    int denominator = std::atoi(new_text);
                    if (denominator == 0) {
                        throw SyntaxError("The denominator should not be zero!");
                    }
                    int gcd = std::__gcd(numerator, denominator);
                    if (gcd == denominator) {
                        return std::make_unique<NumericLiteralToken>(static_cast<double>(numerator / denominator));
                    }
                    else {
                        return std::make_unique<RationalToken>(numerator / gcd, denominator / gcd);
                    }
                }
            }
            if (std::isdigit(text[0]) || text[0] == '+' || text[0] == '-' || text[0] == '.') {
                try {
                    return std::make_unique<NumericLiteralToken>(std::stod(text));
                } catch (std::invalid_argument& e) {
                }
            }
            return std::make_unique<IdentifierToken>(text);
        }
    }
    return nullptr;
}

std::deque<TokenPtr> Tokenizer::tokenize() {
    std::deque<TokenPtr> tokens;
    int pos = 0;
    while (true) {
        auto token = nextToken(pos);
        if (!token) {
            break;
        }
        tokens.push_back(std::move(token));
    }
    return tokens;
}

std::deque<TokenPtr> Tokenizer::tokenize(const std::string& input) {
    return Tokenizer(input).tokenize();
}
