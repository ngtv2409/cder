#include "CLI/CLI.hpp"
#include <string>
#include <vector>
#ifndef CDER_MARK_HPP

namespace cder {
    namespace mark {
        namespace cli {
            struct AddOpt {
                std::string alias;
                std::string path;
            };
            struct GetOpt {
                std::string alias;
            };

            void setup_options(CLI::App &app);
        }

        struct Bookmark {
            std::string alias;
            std::string path;
        };
        Bookmark getMark(std::string &alias);
        /*
            Get all bookmarks from db as a vector
         */
        std::vector<Bookmark> getMarks();
        int pushMark(Bookmark &);
    }
}

#endif // !CDER_MARK_HPP
