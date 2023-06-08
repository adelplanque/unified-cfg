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

#ifndef __parser_h__
#define __parser_h__

#include <filesystem>
#include <list>
#include <memory>
#include <regex>
#include <string>
#include <iostream>

#include "core.h"
#include "value.h"

CFG_BEGIN_NAMESPACE

/**
 * Parse a config file.
 */
class parser_t
{
public:
    typedef std::shared_ptr<parser_t> ptr;

    typedef std::pair<comments_t, option_values_t> pair_comment_option_t;
    typedef std::map<std::string, pair_comment_option_t> map_options_t;
    typedef std::pair<comments_t, map_options_t> pair_comment_group_t;
    typedef std::map<std::string, pair_comment_group_t> options_t;

    parser_t() {};
    void load_file(std::shared_ptr<std::filesystem::path> filename);
    bool empty() { return options.empty(); }
    options_t& get_options() { return options; };

private:

    static const std::regex group_regex;
    static const std::regex comment_regex;
    static const std::regex option_regex;
    static const std::regex empty_regex;

    options_t options;
};

CFG_END_NAMESPACE

#endif
