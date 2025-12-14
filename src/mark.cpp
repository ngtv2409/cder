#include "main.hpp"
#include "mark.hpp"
#include "argparser.hpp"
using namespace cder::mark;

#include "PCH/rapidjson_pch.hpp"
namespace rj = rapidjson;

#include "PCH/cli11_pch.hpp"

#include "PCH/std_pch.hpp"

void cli::setup_options(CLI::App &app) {
    CLI::App *mark = app.add_subcommand("mark", "directory bookmarks");
    mark->require_subcommand(1);
    {
        static AddOpt addopt{std::string("\0"), std::string("\0")};
        CLI::App *add = mark->add_subcommand("add", "add a bookmark");
        add->add_option("alias", addopt.alias, "the name of the mark")->required();
        add->add_option("path", addopt.path, "the path to mark")->required();
        add->add_option("-c, --categories", addopt.categories,
                "the categories to add (space seperated)\n"
                "Note: remember to quote from shell. -c \"cat1 cat2\""
        )->default_val("default");
        add->callback([]() {
            std::vector<std::string> cat = cder::argparser::vector_str(
                addopt.categories);
            if (cat.empty()) {
                std::cerr <<
                    "ERR Error: No categories specified" <<std::endl;
                Error = 1;
                return;
            }

            Bookmark m{addopt.alias, addopt.path};
            Error = pushMark(m, cat);
            if (Error != 0) {
                std::cout << "SUC" << std::endl;
            }
        });
    }

    static GetOpt getopt{std::string("\0")};
    CLI::App *get = mark->add_subcommand("get", "get the path of a bookmark");
    get->add_option("alias", getopt.alias, "the name of the mark")->required();
    get->callback([]() {
        Bookmark m = getMark(getopt.alias);
        if (m.alias.empty()) {
            std::cerr << "ERR Error: No such bookmark in database" << std::endl;
            Error = 1;
        } else {
            std::cout << "SUC PATH \"" << m.path << "\"" << std::endl;
        }
    });
}

namespace cder::mark {
Bookmark getMark(std::string &alias) {
    const rj::Document &db = cder::db::marks;
    if (! db.HasMember("bookmarks")) {
        return {std::string{"\0"},std::string{"\0"}};
    }
    const rj::Value &obj = db["bookmarks"];
    if (! obj.IsObject()) {
        return {std::string{"\0"},std::string{"\0"}};
    }
    if (! obj.HasMember(alias.c_str())) {
        return {std::string{"\0"},std::string{"\0"}};
    }
    if (! obj[alias.c_str()].IsString()) {
        return {std::string{"\0"},std::string{"\0"}};
    }
    Bookmark m{alias, obj[alias.c_str()].GetString()};
    return m;
}

std::vector<Bookmark> getMarks() {
    const rj::Document &db = cder::db::marks;
    if (! db.HasMember("bookmarks")) {
        return std::vector<Bookmark>({});
    }
    const rj::Value &obj = db["bookmarks"];
    if (! obj.IsObject()) {
        return std::vector<Bookmark>({});
    }
    std::vector<Bookmark> vec(obj.Size());
    for (auto& m : obj.GetObject()) {
        if (! (
                m.name.IsString() && m.value.IsString()
        )) {
            continue;
        }
        vec.push_back((Bookmark){
            m.name.GetString(), m.value.GetString()
        });
    }
    return vec;
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
}
