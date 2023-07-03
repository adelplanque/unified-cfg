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

#include <vector>
#include <cstdlib>

#include "core.h"
#include "config.h"

#include <boost/algorithm/string.hpp>

#include <iostream>

CFG_BEGIN_NAMESPACE


config_t config_t::instance{};

std::string env_to_string(const char* name)
{
    std::string result;
    char* raw = std::getenv(name);
    if (raw != nullptr) {
        result = raw;
    }
    return result;
}


config_t::config_t()
{
    std::vector<std::string> paths;
    auto env_config_path = env_to_string("CONFIG_PATH");
    boost::split(paths, env_config_path, boost::is_any_of(":"));
    for (const auto& path: paths) {
        if (! path.empty()) {
            config_path.push_back(path);
        }
    }
    config_name = env_to_string("CONFIG_NAME");
}


void config_t::push_front_config_path(const std::string& path)
{
    config_path.push_front(path);
}


void config_t::push_back_config_path(const std::string& path)
{
    config_path.push_back(path);
}

CFG_END_NAMESPACE
