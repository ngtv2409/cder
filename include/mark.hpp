#include <vector>
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
            struct ListOpt {
                std::string categories;
            };

            void setup_options(CLI::App &app);
        }

        struct Bookmark {
            std::string alias;
            std::string path;
        };
        Bookmark getMark(std::string &alias, std::vector<std::string> categories, std::string &incat);
        int pushMark(Bookmark &, std::vector<std::string> categories);

        /* Get all marks in a category */
        std::vector<Bookmark> getInCat(std::string &cat);
    }
}

#endif // !CDER_MARK_HPP
