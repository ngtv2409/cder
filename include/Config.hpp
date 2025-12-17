#ifndef CDER_CONFIG_HPP
#define CDER_CONFIG_HPP

#include <cstddef>

namespace cder {
    namespace Config {
        static constexpr std::size_t BufferSize =
            #ifndef CDER_CFSET_BUFFERSIZE
                65536
            #else
                CDER_CFSET_BUFFERSIZE
            #endif
                ;
    }
}

#endif // !CDER_CONFIG_HPP
