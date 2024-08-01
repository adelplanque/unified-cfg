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
#include <set>

#include "core.h"
#include "hardware.h"

CFG_BEGIN_NAMESPACE

const std::regex cpuinfo_processor_regex {R"(^processor\s+:\s+(\d+)\s*$)"};
const std::regex cpuinfo_physicalid_regex {R"(^physical id\s+:\s+(\d+)\s*$)"};
const std::regex cpuinfo_coreid_regex {R"(^core id\s+:\s+(\d+)\s*$)"};
const std::string cpuinfo_file {"/proc/cpuinfo"};


int _cpu_cores(const std::string& filepath, bool logical, int min)
{
    std::ifstream is { filepath };
    std::string line;
    int logical_cores = 0;
    std::set<std::pair<int, int>> physical_cores {};
    int processor_id = -1;
    int physical_id = -1;
    int core_id = -1;
    while (std::getline(is, line)) {
        std::smatch m;
        if (std::regex_match(line, m, cpuinfo_processor_regex)) {
            if (processor_id != -1) {
                physical_cores.emplace(physical_id, core_id);
                physical_id = -1;
                core_id = -1;
            }
            processor_id = std::stoi(m[1]);
            logical_cores += 1;
        } else if (std::regex_match(line, m, cpuinfo_physicalid_regex)) {
            physical_id = std::stoi(m[1]);
        } else if (std::regex_match(line, m, cpuinfo_coreid_regex)) {
            core_id = std::stoi(m[1]);
        }
    }
    if (processor_id != -1) {
        physical_cores.emplace(physical_id, core_id);
    }
    if (logical) {
        return std::max(min, logical_cores);
    } else {
        return std::max(min, (int) physical_cores.size());
    }
}

int cpu_cores(bool logical, int min)
{
    return _cpu_cores(cpuinfo_file, logical, min);
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
