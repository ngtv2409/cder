#ifndef CDER_MARK_HPP

#include "PCH/cli11_pch.hpp"
#include "PCH/std_pch.hpp"

namespace cder {
    namespace mark {
        namespace cli {
            struct CommonOpt {
                std::string categories;
            };

            struct AddOpt {
                std::string alias;
                std::string path;
            };
            struct GetOpt {
                std::string alias;
            };
            struct ListOpt {
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

        std::vector<std::string> getCats();
    }
}

#endif // !CDER_MARK_HPP
