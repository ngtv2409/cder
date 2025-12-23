#ifndef CDER_MARK_HPP
#define CDER_MARK_HPP

#include "protocol.hpp"

#include "PCH/cli11_pch.hpp"
#include "PCH/std_pch.hpp"

namespace cder {
    namespace mark {
        namespace cli {
            struct CommonOpt {
                std::string categories;
                int all_cats;
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
            struct RmOpt {
                std::string alias;
            };

            void setup_options(CLI::App &app);
        }

        struct Bookmark {
            std::string alias;
            std::string path;
        };
        Bookmark getMark(std::string &alias, std::string &category, protocol::Error &e);
        int pushMark(Bookmark &, std::string &category);

        /* Get all marks in a category */
        std::vector<Bookmark> getInCat(std::string &cat, protocol::Error &e);

        std::vector<std::string> getCats();
        void removeMark(std::string &alias, std::string &category, protocol::Error &e);
    }
}

#endif // !CDER_MARK_HPP
