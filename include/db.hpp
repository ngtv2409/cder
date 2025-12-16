#ifndef CDER_MAIN_HPP

#include "PCH/rapidjson_pch.hpp"
#include "PCH/std_pch.hpp"

namespace cder {
    namespace db {
        struct _db {
            rapidjson::Document marks;
        }; 
        extern _db dbcol;

        int setup_db(const std::string &dbdir);
        int finalize_db(const std::string &dbdir);
    }
}

#endif // !CDER_MAIN_HPP
