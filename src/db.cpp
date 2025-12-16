#include "db.hpp"
#include "Config.hpp"

#include "PCH/std_pch.hpp"
#include "PCH/rapidjson_pch.hpp"

namespace fs = std::filesystem;
namespace rj = rapidjson;

cder::db::_db cder::db::dbcol;

char buffer[cder::Config::BufferSize];

using namespace cder::db;

int cder::db::setup_db(const std::string &dbdir) {
    if (! fs::exists(dbdir)) {
        try {
            fs::create_directory(dbdir);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "ERR Error: " << e.what() << std::endl;
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
            std::perror("ERR Error");
            return 1;
        }
        rj::FileReadStream is(fp, buffer, sizeof(buffer));
        if (dbcol.marks.ParseStream(is).HasParseError()) {
            std::cerr << "ERR Error: " << 
            rj::GetParseError_En(dbcol.marks.GetParseError()) << std::endl;
        }
        // is not object, ignore and set empty
        if (! dbcol.marks.IsObject()) {
            dbcol.marks.SetObject();
        }
        std::fclose(fp);
    }
    return 0;
}

int cder::db::finalize_db(const std::string &dbdir) {
/* Finallize database */
    {
        fs::path db_marks_path = fs::path(dbdir) / "marks.json";
        std::FILE *fp = std::fopen(db_marks_path.c_str(), "wb");
        if (! fp) {
            std::perror("ERR Error");
            return 1;
        }
        rj::FileWriteStream os(fp, buffer, sizeof(buffer));
        rj::Writer<rj::FileWriteStream> writer(os);
        dbcol.marks.Accept(writer);
        std::fclose(fp);
    }
    return 0;
}
