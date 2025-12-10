#include "Config.hpp"

#include "rapidjson/error/en.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
namespace rj = rapidjson;

#include "CLI/CLI.hpp"

#include <cstdio>
#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, const char **argv) {
/* rapidjson stream buffer */
    char buffer[cder::Config::BufferSize];

/*
    I don't think abstract this with a function is a good idea
    because it requires extra steps to scope the variables.
    It does make the function longer but I'll let main do all the setup
*/
/*{{{ Setup database */
    char *dbdir_r = std::getenv("CDER_DB_PATH");
    if (! dbdir_r) {
        std::cerr << "Error: CDER_DB_PATH must be set" << std::endl;
        return 1;
    }
    std::string dbdir(dbdir_r);    
    if (dbdir.empty()) {
        std::cerr << "Error: CDER_DB_PATH must be set" << std::endl;
        return 1;
    }
    if (! fs::exists(dbdir)) {
        try {
            fs::create_directory(dbdir);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << "\n";
            return 1;
        }
    }

    /* Read DB */
    rj::Document db_marks;
    fs::path db_marks_path = fs::path(dbdir) / "marks.json";
    if (! fs::exists(db_marks_path)) {
        db_marks.SetObject();
    } else {
        std::FILE *fp = std::fopen(db_marks_path.c_str(), "rb");
        if (! fp) {
            std::perror("Error");
            return 1;
        }
        rj::FileReadStream is(fp, buffer, sizeof(buffer));
        if (db_marks.ParseStream(is).HasParseError()) {
            fprintf(stderr, "\nError(offset %u): %s\n", 
            (unsigned)db_marks.GetErrorOffset(),
            rj::GetParseError_En(db_marks.GetParseError()));
        }
        std::fclose(fp);
    }
/* End Setup database }}}*/


    return 0;
}
