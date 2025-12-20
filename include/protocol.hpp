#ifndef CDER_PROTOCOL_HPP
#define CDER_PROTOCOL_HPP

#include "PCH/std_pch.hpp"

#define FIELD(name, val) ((cder::protocol::Field){name, val})
#define ERRORF(name, act, msg) FIELD("CODE", name), FIELD("ACTION", act), FIELD("MESSAGE", msg)

namespace cder::protocol {
    struct Error {
        int hasError;
        std::string name;
        std::string action; // The action tried to perform which caused it (high level)
        std::string message;

        void setError(const std::string &name, const std::string &action, const std::string &message) {
            this->hasError = 1;
            this->name = name;
            this->action = action;
            this->message = message;
        }
    };

    struct Field {
        std::string name;
        std::string value;
    };

    void send_message(const std::vector<Field> &fields);
}

#endif // !CDER_PROTOCOL_HPP
