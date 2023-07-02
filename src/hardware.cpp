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
#include <regex>

#include "core.h"
#include "hardware.h"

CFG_BEGIN_NAMESPACE

const std::regex cpuinfo_regex {R"(^cpu cores\s+:\s+(\d+)\s*$)"};

int cpu_cores()
{
    std::ifstream is { "/proc/cpuinfo" };
    std::string line;
    while (std::getline(is, line)) {
        std::smatch m;
        if (std::regex_match(line, m, cpuinfo_regex)) {
            return std::stoi(m[1]);
        }
    }
    return -1;
}

long int meminfo(const std::string& key)
{
    std::ifstream is {"/proc/meminfo"};
    std::string str;
    while (getline(is, str)) {
        if (str.rfind(key, 0) == 0 && str[key.size()] == ':') {
            return std::atol(str.c_str() + key.size() + 1);
        }
    }
    return -1;
}

CFG_END_NAMESPACE
