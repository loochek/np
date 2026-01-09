#include <fstream>
#include <vector>
#include <iostream>
#include <string>

bool IsSubstring(const std::string& inner, const std::string& outer) {
    if (inner.size() > outer.size()) {
        return false;
    }

    return outer.find(inner) != std::string::npos;
}

int main() {
    std::ifstream input("input.txt");
    std::vector<std::string> strings;

    std::string s;
    while (input >> s) {
        strings.push_back(std::move(s));
    }
    input.close();

    std::ifstream output("output.txt");
    std::string superstring;
    std::getline(output, superstring);
    output.close();

    std::cout << "Input: " << strings.size() << " strings\n";
    std::cout << "Superstring length: " << superstring.size() << " codepoints\n\n";

    for (const auto& str : strings) {
        if (!IsSubstring(str, superstring)) {
            std::cout << "Error - string '" << str << "' is not a substring of the result\n";
            return 1;
        }
    }

    std::cout << "Correct - all strings are substrings of the superstring\n";
    return 0;
}
