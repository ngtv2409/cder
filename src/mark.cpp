#include "main.hpp"
#include "rapidjson/rapidjson.h"
#include "mark.hpp"
using namespace cder::mark;

#include "rapidjson/document.h"
namespace rj = rapidjson;

#include "CLI/CLI.hpp"

#include <vector>
#include <filesystem>
#include <iostream>

void cli::setup_options(CLI::App &app) {
    CLI::App *mark = app.add_subcommand("mark", "directory bookmarks");
    mark->require_subcommand(1);

    AddOpt addopt{std::string("\0"), std::string("\0")};
    CLI::App *add = mark->add_subcommand("add", "add a bookmark");
    add->add_option("alias", addopt.alias, "the name of the mark")->required();
    add->add_option("path", addopt.path, "the path to mark")->required();
    add->callback([&addopt]() {
        Bookmark m{addopt.alias, addopt.path};
        Error = pushMark(m);
    });
}

namespace cder::mark {
std::vector<Bookmark> getMarks() {
    const rj::Document &db = cder::db::marks;
    if (! db.HasMember("bookmarks")) {
        return std::vector<Bookmark>({});
    }
    const rj::Value &obj = db["bookmarks"];
    // expects array
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

int pushMark(Bookmark &m) {
    rj::Document &db = cder::db::marks;
    auto &alloc = db.GetAllocator();
    if (! db.HasMember("bookmarks")) {
        db.AddMember(rj::Value("bookmarks", alloc),
                     rj::Value(rj::kObjectType),
                     alloc);
    }
    rj::Value &obj = db["bookmarks"];
    // expects array
    if (! obj.IsObject()) {
        obj.SetObject();
    }

    auto path = std::filesystem::absolute(m.path);
    if (! std::filesystem::exists(path)) {
        std::cerr << "Error: no such directory\n";
        return 1;
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
    return 0;
}
}
