#include "protocol.hpp"

#include <iostream>


void cder::protocol::send_message(const std::vector<Field> &fields) {
    for (auto &f : fields) {
        std::cout
                  << f.name
                  << '\0'
                  << f.value
                  << '\0';
    }
    std::cout << '\n';
}
