#include "protocol.hpp"

#include <iostream>


void cder::protocol::send_message(const std::vector<Field> &fields) {
    /*
        0x1F: ASCII US unit seperator
        0x1E: ASCII RS record seperator
    */
    for (auto &f : fields) {
        std::cout
                  << f.name
                  << '\x1F'
                  << f.value
                  << '\x1F';
    }
    std::cout << '\x1E';
}
