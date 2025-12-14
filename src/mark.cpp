#include "main.hpp"
#include "mark.hpp"
#include "argparser.hpp"
#include <vector>
using namespace cder::mark;

#include "PCH/rapidjson_pch.hpp"
namespace rj = rapidjson;

#include "PCH/cli11_pch.hpp"

#include "PCH/std_pch.hpp"

void cli::setup_options(CLI::App &app) {
    static CommonOpt commonopt;
    CLI::App *mark = app.add_subcommand("mark", "directory bookmarks");
    mark->require_subcommand(1);
    mark->add_option("-c, --categories", commonopt.categories,
            "the categories to operate on (space seperated)\n"
            "Note: remember to quote from shell. -c \"cat1 cat2\""
    )->default_val("default");
    mark->add_flag("-a, --all-categories", commonopt.all_cats, "include all categories");

    // {{{ Add
    static AddOpt addopt;
    CLI::App *add = mark->add_subcommand("add", "add a bookmark")->fallthrough();
    add->add_option("alias", addopt.alias, "the name of the mark")->required();
    add->add_option("path", addopt.path, "the path to mark")->required();
    add->callback([]() {
        std::vector<std::string> cats;
        if (! commonopt.all_cats) {
            cats = cder::argparser::vector_str(
                commonopt.categories);
            if (cats.empty()) {
                std::cerr <<
                    "ERR Error: No categories specified" <<std::endl;
                Error = 1;
                return;
            }
        } else {
            cats = getCats();
        }

        Bookmark m{addopt.alias, addopt.path};
        Error = pushMark(m, cats);
        if (Error != 0) {
            std::cout << "SUC" << std::endl;
        }
    });
    /// }}}

    // {{{ Get
    static GetOpt getopt;
    CLI::App *get = mark->add_subcommand("get", "get the path of a bookmark")->fallthrough();
    get->add_option("alias", getopt.alias, "the name of the mark")->required();
    get->callback([]() {
        std::vector<std::string> cats;
        if (! commonopt.all_cats) {
            cats = cder::argparser::vector_str(
                commonopt.categories);
            if (cats.empty()) {
                std::cerr <<
                    "ERR Error: No categories specified" <<std::endl;
                Error = 1;
                return;
            }
        } else {
            cats = getCats();
        }

        std::string incat;
        Bookmark m = getMark(getopt.alias, cats, incat);
        if (m.alias.empty()) {
            std::cerr << "ERR Error: No such bookmark in database" << std::endl;
            Error = 1;
        } else {
            std::cout << "SUC PATH \"" << m.path << "\"" 
                      << " CAT \"" << incat << "\"" 
                      << std::endl;
        }
    });
    // }}}

    /// {{{ List
    static ListOpt listopt;
    CLI::App *list = mark->add_subcommand("list", "list all marks in categories")->fallthrough();

    list->callback([]() 
    {
        std::vector<std::string> cats;
        if (! commonopt.all_cats) {
            cats = cder::argparser::vector_str(
                commonopt.categories);
            if (cats.empty()) {
                std::cerr <<
                    "ERR Error: No categories specified" <<std::endl;
                Error = 1;
                return;
            }
        } else {
            cats = getCats();
        }

        std::cout << "HELP List of categories:" << std::endl;
        for (auto &c : cats) {
            std::cout << c << ":\n";
            std::vector<Bookmark> v = getInCat(c);
            for (auto &m : v) {
                std::cout << "    " << m.alias << ": " << m.path << '\n';
            }
        }
    });
    // }}}
}

namespace cder::mark {
Bookmark getMark(std::string &alias, std::vector<std::string> categories, std::string &incat) {
    const rj::Document &db = cder::db::marks;
    Bookmark m{"\0"};
    for (auto &cat : categories) {
        if (! db.HasMember(cat.c_str())) {
            continue;
        }
        const rj::Value &obj = db[cat.c_str()];
        if (! obj.IsObject()) {
            continue;
        }
        if (! obj.HasMember(alias.c_str())) {
            continue;
        }
        if (! obj[alias.c_str()].IsString()) {
            continue;
        }
        m.alias = alias;
        m.path = obj[alias.c_str()].GetString();
        incat = cat;
        break;
    }
    return m;
}

int pushMark(Bookmark &m, std::vector<std::string> categories) {
    rj::Document &db = cder::db::marks;
    auto &alloc = db.GetAllocator();

    auto path = std::filesystem::absolute(m.path);
    if (! std::filesystem::exists(path)) {
        std::cerr << "ERR Error: no such directory" << std::endl;
        return 1;
    }

    for (const std::string &cat : categories) {
        if (! db.HasMember(cat.c_str())) {
            db.AddMember(rj::Value(cat.c_str(), alloc),
                         rj::Value(rj::kObjectType),
                         alloc);
        }
        rj::Value &obj = db[cat.c_str()];
        if (! obj.IsObject()) {
            obj.SetObject();
        }

        if (obj.HasMember(m.alias.c_str())) {
            obj[m.alias.c_str()] = rj::Value(path.c_str(), alloc);
        } else {
            obj.AddMember(
                rj::Value(m.alias.c_str(), alloc),
                rj::Value(path.c_str(), alloc),
                alloc
            );
        }
    }
    return 0;
}
std::vector<Bookmark> getInCat(std::string &cat) {
    const rj::Document &db = cder::db::marks;
    std::vector<Bookmark> v;
    const rj::Value &obj = db[cat.c_str()];
    if (! obj.IsObject()) {
        return v;
    }
    for (auto &member : obj.GetObject()) {
        if (! member.value.IsString()) {
            continue;
        }
        Bookmark m;
        m.alias = member.name.GetString();
        m.path = member.value.GetString();
        v.push_back(m);
    }
    return v;
}

std::vector<std::string> getCats() {
    const rj::Document &db = cder::db::marks;
    std::vector<std::string> v;
    for (auto &member : db.GetObject()) {
        v.push_back(member.name.GetString());
    }
    return v;
}
}
