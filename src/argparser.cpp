#include "argparser.hpp"

#include <sstream>

std::vector<std::string> cder::argparser::vector_str(const std::string &s) {
    std::vector<std::string> v;
    std::istringstream ss(s);
    std::string temp;
    while (ss >> temp) {
        v.push_back(temp);
    }
    return v;
}
