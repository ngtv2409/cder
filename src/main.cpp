#include "main.hpp"
#include "VERSION.hpp"
#include "db.hpp"
#include "mark.hpp"

#include "PCH/cli11_pch.hpp"

#include "PCH/std_pch.hpp"

int cder::Error = 0;

class HELPPrependFmt : public CLI::Formatter {
  public:
      std::string make_description(const CLI::App *app) const override {
        return std::string("HELP ") + app->get_description();
      }
};

int main(int argc, const char **argv) {
    char *dbdir_r = std::getenv("CDER_DB_PATH");
    // exists (not NULL) and not empty (first char not \0)
    if (! ( dbdir_r && *dbdir_r )) {
        std::cerr << "ERR Error: CDER_DB_PATH must be set\n";
    }
    std::string dbdir{dbdir_r};

    int setupec = cder::db::setup_db(dbdir);
    if (setupec) {
        return setupec;
    }

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

    int finalec = cder::db::finalize_db(dbdir);
    if (finalec) {
        return finalec;
    }

    return cder::Error;
}
