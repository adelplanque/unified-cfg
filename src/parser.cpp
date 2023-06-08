// Copyright (C) 2022 Alain Delplanque

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>

#include <fmt/format.h>

#include "core.h"
#include "parser.h"

CFG_BEGIN_NAMESPACE

class file_queue_t
{
public:
    file_queue_t(const std::filesystem::path& filename)
        : is(filename)
    {}

    bool getline(std::string& line) {
        if (q.empty()) {
            return static_cast<bool>(std::getline(is, line));
        } else {
            line = std::move(q.front());
            q.pop_front();
            return true;
        }
    }

    void rollback(std::string&& line) {
        q.push_back(std::move(line));
    }


private:
    std::ifstream is;
    std::list<std::string> q;

};

const std::regex parser_t::group_regex {R"(^\s*\[([_a-zA-Z]\w*)\]\s*$)"};
const std::regex parser_t::comment_regex {R"(^\s*[;\#](.*)$)"};
const std::regex parser_t::option_regex {
    R"(^\s*([_a-zA-Z]\w*)(\[([a-zA-Z]\w*)\])?\s*=\s*(.*)$)" };
const std::regex parser_t::empty_regex {R"(^\s*$)"};

void parser_t::load_file(std::shared_ptr<std::filesystem::path> filename)
{
    file_queue_t infile { *filename };
    std::string line;
    int lineno = 0;
    std::string group_name = "DEFAULT";
    std::smatch m;
    std::list<std::string> comments;

    while (infile.getline(line)) {
        lineno++;
        if (std::regex_match(line, empty_regex)) {
            continue;
        }
        if (std::regex_match(line, m, group_regex)) {
            auto& [group_comments, group_map] = this->options[m[1]];
            if (! comments.empty()) {
                group_comments.append(std::move(comments));
            }
            group_name = std::move(m[1]);
            continue;
        }
        if (std::regex_match(line, m, comment_regex)) {
            comments.push_back(std::move(m[1]));
            continue;
        }
        if (std::regex_match(line, m, option_regex)) {
            auto& [group_comment, group_map] = options[group_name];
            std::string option_name = m[1];
            std::string config_name = m[3];
            std::string value = m[4];
            auto [it, inserted] = group_map.try_emplace(option_name, comments_t(), option_name);
            auto& [option_comments, values] = it->second;
            size_t indent = line.find_first_not_of(' ');
            while (infile.getline(line)) {
                if (line.empty()) {
                    break;
                }
                size_t next_indent = line.find_first_not_of(' ');
                if (next_indent != std::string::npos && next_indent > indent) {
                    value += '\n';
                    value += line.substr(next_indent);
                } else {
                    infile.rollback(std::move(line));
                    break;
                }
            }
            values.add_value(filename, config_name, value);
            if (! comments.empty()) {
                option_comments.append(std::move(comments));
            }
            continue;
        }
        throw std::runtime_error(fmt::format("Error at {}:{}:\n\t{}\n", filename->native(), lineno, line));
    }
}

CFG_END_NAMESPACE
