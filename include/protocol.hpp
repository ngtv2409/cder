#ifndef CDER_PROTOCOL_HPP
#define CDER_PROTOCOL_HPP

#include "PCH/std_pch.hpp"

namespace cder::protocol {
    struct Error {
        int hasError;
        std::string name;
        std::string message;

        void setError(std::string &name, std::string &message) {
            this->hasError = 1;
            this->name = name;
            this->message = message;
        }
    };

    void send_message(std::vector<std::pair<std::string, std::string>> &fields);
}

#endif // !CDER_PROTOCOL_HPP
