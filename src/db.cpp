#include "db.hpp"
#include "Config.hpp"

#include "PCH/std_pch.hpp"
#include "PCH/rapidjson_pch.hpp"
#include "protocol.hpp"
#include "rapidjson/error/en.h"
#include <cerrno>

namespace fs = std::filesystem;
namespace rj = rapidjson;

cder::db::_db cder::db::dbcol;

char buffer[cder::Config::BufferSize];

using namespace cder::db;

int cder::db::setup_db(const std::string &dbdir, protocol::Error &e) {
    if (! fs::exists(dbdir)) {
        try {
            fs::create_directory(dbdir);
        } catch (const fs::filesystem_error& fse) {
            e.setError("fs", "creating database directory", fse.what());
            return 1;
        }
    }

    /* Read DB */
    fs::path db_marks_path = fs::path(dbdir) / "marks.json";
    if (! fs::exists(db_marks_path)) {
        dbcol.marks.SetObject();
    } else {
        std::FILE *fp = std::fopen(db_marks_path.c_str(), "rb");
        if (! fp) {
            e.setError("fs", "opening marks.json", std::strerror(errno));
            return 1;
        }
        rj::FileReadStream is(fp, buffer, sizeof(buffer));
        if (dbcol.marks.ParseStream(is).HasParseError()) {
            e.setError("json", "parsing marks.json", 
                        rj::GetParseError_En(dbcol.marks.GetParseError()));
            return 1;
        }
        // is not object, ignore and set empty
        if (! dbcol.marks.IsObject()) {
            dbcol.marks.SetObject();
        }
        std::fclose(fp);
    }
    return 0;
}

int cder::db::finalize_db(const std::string &dbdir, protocol::Error &e) {
/* Finallize database */
    {
        fs::path db_marks_path = fs::path(dbdir) / "marks.json";
        std::FILE *fp = std::fopen(db_marks_path.c_str(), "wb");
        if (! fp) {
            e.setError("fs", "opening marks.json", std::strerror(errno));
            return 1;
        }
        rj::FileWriteStream os(fp, buffer, sizeof(buffer));
        rj::Writer<rj::FileWriteStream> writer(os);
        dbcol.marks.Accept(writer);
        std::fclose(fp);
    }
    return 0;
}
