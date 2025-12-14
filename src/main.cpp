#include "Config.hpp"
#include "main.hpp"
#include "VERSION.hpp"
#include "mark.hpp"

#include "PCH/rapidjson_pch.hpp"
namespace rj = rapidjson;

#include "PCH/cli11_pch.hpp"

#include "PCH/std_pch.hpp"
namespace fs = std::filesystem;

rj::Document cder::db::marks;
int cder::Error = 0;

class HELPPrependFmt : public CLI::Formatter {
  public:
      std::string make_description(const CLI::App *app) const override {
        return std::string("HELP ") + app->get_description();
      }
};

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
        std::cerr << "ERR Error: CDER_DB_PATH must be set" << std::endl;
        return 1;
    }
    std::string dbdir(dbdir_r);    
    if (dbdir.empty()) {
        std::cerr << "ERR Error: CDER_DB_PATH must be set" << std::endl;
        return 1;
    }
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
        cder::db::marks.SetObject();
    } else {
        std::FILE *fp = std::fopen(db_marks_path.c_str(), "rb");
        if (! fp) {
            std::perror("ERR Error");
            return 1;
        }
        rj::FileReadStream is(fp, buffer, sizeof(buffer));
        if (cder::db::marks.ParseStream(is).HasParseError()) {
            std::cerr << "ERR Error: " << 
            rj::GetParseError_En(cder::db::marks.GetParseError()) << std::endl;
        }
        // is not object, ignore and set empty
        if (! cder::db::marks.IsObject()) {
            cder::db::marks.SetObject();
        }
        std::fclose(fp);
    }
/* End Setup database }}}*/

/* Setup CLI */
    CLI::App app{"cder is a smart wrapper on cd", "cder"};
    app.require_subcommand(1);
    app.formatter(std::make_shared<HELPPrependFmt>());

    app.set_version_flag("-v, --version", std::string("cder version ") + cder::Version);
    app.set_help_all_flag("--help-all", "show all help of each subcommands and exit");

    cder::mark::cli::setup_options(app);

    try {
        (app).parse(argc, argv);
    } catch(const CLI::ParseError &e) {
        if (e.get_exit_code() != 0)
            std::cerr << "ERR ";
        return (app).exit(e);
    }
/* End Setup CLI */

/* Finallize database */
    {
        std::FILE *fp = std::fopen(db_marks_path.c_str(), "wb");
        if (! fp) {
            std::perror("ERR Error");
            return 1;
        }
        rj::FileWriteStream os(fp, buffer, sizeof(buffer));
        rj::Writer<rj::FileWriteStream> writer(os);
        cder::db::marks.Accept(writer);
        std::fclose(fp);
    }

    return cder::Error;
}
