#include "main.hpp"
#include "mark.hpp"
#include "argparser.hpp"
#include "db.hpp"
#include "protocol.hpp"
using namespace cder::mark;

using cder::db::dbcol;

#include "PCH/rapidjson_pch.hpp"
namespace rj = rapidjson;

#include "PCH/cli11_pch.hpp"

#include "PCH/std_pch.hpp"

#define quote(w) '\"' << w << '\"'

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
                protocol::send_message({
                    ERRORF("cli", "getting categories", "no category specified")    
                });
                ErrorCode = 1;
                return;
            }
        } else {
            cats = getCats();
        }

        auto path = std::filesystem::absolute(addopt.path);
        if (! std::filesystem::exists(path)) {
            protocol::send_message({ERRORF("fs", "validating", path.string() + " does not exist")});
            ErrorCode = 1;
            return;
        }

        Bookmark m{addopt.alias, path};
        for (auto &cat : cats) {
            pushMark(m, cat);
            protocol::send_message({
                {"CODE", "nul"},
                {"CAT", cat},
                {"ALIAS", addopt.alias},
                {"PATH", path},
            });
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
                protocol::send_message({
                    ERRORF("cli", "getting categories", "no category specified")    
                });
                ErrorCode = 1;
                return;
            }
        } else {
            cats = getCats();
        }

        protocol::Error e;
        for (auto &cat : cats) {
            e.hasError = 0;
            Bookmark m = getMark(getopt.alias, cat, e);
            if (e.hasError) {
                protocol::send_message({
                    ERRORF(e.name, e.action, e.message),
                });
                continue;
            }
            if (m.alias.empty()) {
                continue;
            } else {
                protocol::send_message({
                        {"CODE", "nul"},
                        {"CAT", cat},
                        {"ALIAS", getopt.alias},
                        {"PATH", m.path}
                });
            }
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
                protocol::send_message({
                    ERRORF("cli", "getting categories", "no category specified")    
                });
                ErrorCode = 1;
                return;
            }
        } else {
            cats = getCats();
        }

        protocol::Error e;

        for (auto &c : cats) {
            std::vector<Bookmark> v = getInCat(c, e);
            if (e.hasError) {
                protocol::send_message({
                    ERRORF(e.name, e.action, e.message),
                });
            }

            for (auto &m : v) {
                protocol::send_message({
                    {"CODE", "nul"},
                    {"CAT", c},
                    {"ALIAS", m.alias},
                    {"PATH", m.path}
                });
            }
        }
    });
    // }}}
   
    // {{{ Rm
    static RmOpt rmopt;
    CLI::App *rm = mark->add_subcommand("rm", "remove specified mark in categories")->fallthrough();
    rm->add_option("alias", rmopt.alias, "the name of the mark")->required();

    rm->callback([]() 
    {
        std::vector<std::string> cats;
        if (! commonopt.all_cats) {
            cats = cder::argparser::vector_str(
                commonopt.categories);
            if (cats.empty()) {
                protocol::send_message({
                    ERRORF("cli", "getting categories", "no category specified")    
                });
                ErrorCode = 1;
                return;
            }
        } else {
            cats = getCats();
        }
        
        protocol::Error e;
        for (auto &cat : cats) {
            removeMark(rmopt.alias, cat, e);
            if (e.hasError) {
                protocol::send_message({
                    ERRORF(e.name, e.action, e.message),
                });
                e.hasError = 0;
            } else {
                protocol::send_message({
                    {"CODE", "nul"},
                    {"CAT", cat},
                    {"ALIAS", rmopt.alias}
                });
            }
        }
    });
    // }}}
}

namespace cder::mark {
Bookmark getMark(std::string &alias, std::string &category, protocol::Error &e) {
    const rj::Document &db = dbcol.marks;
    Bookmark m{"\0"};
    if (! db.HasMember(category.c_str())) {
        e.setError(
                "json", "getting member mark::" + category, 
                "member mark::" + category + " does not exists");
        return m;
    }
    const rj::Value &obj = db[category.c_str()];
    if (! obj.IsObject()) {
        e.setError(
                "json", "validating", 
                "member mark::" + category + " is invalid. Expect object");
        return m;
    }
    if (! obj.HasMember(alias.c_str())) {
        // not exists is not error
        return m;
    }
    if (! obj[alias.c_str()].IsString()) {
        e.setError(
                "json", "validating",
                "member mark::" + category + "::" + alias + " is invalid. Expect string");
        return m;
    }
    m.alias = alias;
    m.path = obj[alias.c_str()].GetString();
    return m;
}

int pushMark(Bookmark &m, std::string &category) {
    rj::Document &db = dbcol.marks;
    auto &alloc = db.GetAllocator();

    if (! db.HasMember(category.c_str())) {
        db.AddMember(rj::Value(category.c_str(), alloc),
                     rj::Value(rj::kObjectType),
                     alloc);
    }
    rj::Value &obj = db[category.c_str()];
    if (! obj.IsObject()) {
        obj.SetObject();
    }

    if (obj.HasMember(m.alias.c_str())) {
        obj[m.alias.c_str()] = rj::Value(m.path.c_str(), alloc);
    } else {
        obj.AddMember(
            rj::Value(m.alias.c_str(), alloc),
            rj::Value(m.path.c_str(), alloc),
            alloc
        );
    }
    return 0;
}
std::vector<Bookmark> getInCat(std::string &cat, protocol::Error &e) {
    const rj::Document &db = dbcol.marks;
    std::vector<Bookmark> v;
    if (! db.HasMember(cat.c_str())) {
        e.setError(
                "json", "getting member mark::" + cat, 
                "member mark::" + cat + " does not exists");
        return v;
    }
    const rj::Value &obj = db[cat.c_str()];
    if (! obj.IsObject()) {
        e.setError("json", "validating", "member mark::" + cat + " is invalid. Expect object");
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
    const rj::Document &db = dbcol.marks;
    std::vector<std::string> v;
    for (auto &member : db.GetObject()) {
        v.push_back(member.name.GetString());
    }
    return v;
}

void removeMark(std::string &alias, std::string &category, protocol::Error &e) {
    rj::Document &db = dbcol.marks;

    if (! db.HasMember(category.c_str())) {
        e.setError("json", "validating", "member mark::"+category+" does not exist");
        return;
    }
    rj::Value &obj = db[category.c_str()];
    if (! obj.IsObject()) {
        e.setError("json", "validating", "member mark::"+category+" is invalid. Expect object");
        return;
    }

    if (obj.HasMember(alias.c_str())) {
        obj.GetObject().RemoveMember(alias.c_str());
    }
}
}
