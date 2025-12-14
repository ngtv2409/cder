#ifndef CDER_MARK_HPP

#include "PCH/cli11_pch.hpp"
#include "PCH/std_pch.hpp"

namespace cder {
    namespace mark {
        namespace cli {
            struct AddOpt {
                std::string alias;
                std::string path;
                std::string categories;
            };
            struct GetOpt {
                std::string alias;
                std::string categories;
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
        int pushMark(Bookmark &, std::vector<std::string> categories);
    }
}

#endif // !CDER_MARK_HPP
