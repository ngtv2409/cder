#ifndef CDER_ARGPARSER_HPP

#include "PCH/std_pch.hpp"

/* Implements custom parsers for better arg pattern */

namespace cder {
    namespace argparser {
        /*
            Parses a string v1 v2 ... into a vector
            values must not contain space (identifier-like)
            all spaces are stripped
        */
        std::vector<std::string> vector_str(const std::string &s);
    }
}

#endif // !CDER_ARGPARSER_HPP
