#ifndef CDER_DB_HPP
#define CDER_DB_HPP

#include "PCH/rapidjson_pch.hpp"
#include "PCH/std_pch.hpp"
#include "protocol.hpp"

namespace cder {
    namespace db {
        struct _db {
            rapidjson::Document marks;
        }; 
        extern _db dbcol;

        int setup_db(const std::string &dbdir, protocol::Error &e);
        int finalize_db(const std::string &dbdir, protocol::Error &e);
    }
}

#endif // !CDER_DB_HPP
