#include "CLI/CLI.hpp"
#include "VERSION.hpp"
#include "db.hpp"
#include "mark.hpp"
#include "protocol.hpp"
#include "main.hpp"

#include "PCH/cli11_pch.hpp"

#include "PCH/std_pch.hpp"
#include <sstream>

int cder::ErrorCode = 0;

int main(int argc, const char **argv) {
    char *dbdir_r = std::getenv("CDER_DB_PATH");
    // exists (not NULL) and not empty (first char not \0)
    if (! ( dbdir_r && *dbdir_r )) {
        cder::protocol::send_message(
            {
                ERRORF("config",
                       "getting environmental variable CDER_DB_PATH",
                       "CDER_DB_PATH must be set prior to the usage of cder")
            }
        );
        return 1;
    }
    std::string dbdir{dbdir_r};

    cder::protocol::Error e;

    int setupec = cder::db::setup_db(dbdir, e);
    if (setupec) {
        cder::protocol::send_message(
            {
                ERRORF(e.name,
                       e.action,
                       e.message)
            }
        );
        return setupec;
    }

/* Setup CLI */
    CLI::App app{"cder is a smart wrapper on cd", "cder"};
    app.require_subcommand(1);

    app.set_version_flag("-v, --version", std::string("cder version ") + cder::Version);
    app.set_help_all_flag("--help-all", "show all help of each subcommands and exit");

    app.add_flag_function(
        "--db-path", [&](int) {
            throw CLI::CallForVersion(dbdir, 0);
        },
        "get db path and exit"
    )->trigger_on_parse();
    
    cder::mark::cli::setup_options(app);

    std::ostringstream oss;
    try {
        (app).parse(argc, argv);
    } catch(const CLI::ParseError &e) {
        int ret = app.exit(e, oss, oss);
        if (e.get_exit_code() != 0) {
            cder::protocol::send_message(
                {
                    ERRORF("cli", "parsing commandline arguments", oss.str())
                }
            );
        } else {
            cder::protocol::send_message(
            {
                {"CODE", "help"},
                {"MESSAGE", oss.str()}
            });
        }
        return ret;
    }
/* End Setup CLI */

    int finalec = cder::db::finalize_db(dbdir, e);
    if (finalec) {
        cder::protocol::send_message(
            {
                ERRORF(e.name,
                       e.action,
                       e.message)
            }
        );
        return finalec;
    }

    return cder::ErrorCode;
}
