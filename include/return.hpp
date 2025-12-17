#ifndef CDER_ERROR_HPP

namespace cder {
    // return value with error infomation
    template <typename additional_field>
    struct Return {
        int code;
        char * message;

        additional_field val;
    };
}

#endif // !CDER_ERROR_HPP
