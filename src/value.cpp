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

#include <boost/algorithm/string/join.hpp>
#include <fmt/format.h>

#include "core.h"
#include "value.h"

CFG_BEGIN_NAMESPACE

void dedent(std::list<std::string>& lines)
{
    if (lines.empty()) {
        return;
    }
    int dedent = -1;
    for (auto& line : lines) {
        int spaces = line.find_first_not_of(' ');
        if (dedent == -1 || dedent > spaces) {
            dedent = spaces;
        }
    }
    for (auto& line : lines) {
        line.erase(0, dedent);
    }
}

void comments_t::append(std::list<std::string>&& lines)
{
    dedent(lines);
    if (! this->lines.empty()) {
        this->lines.push_back("");
    }
    this->lines.splice(this->lines.end(), lines);
}

std::string comments_t::format()
{
    std::string result;
    if (this->lines.empty()) {
        return result;
    }
    result = boost::algorithm::join(this->lines, "\n");
    result += "\n";
    return result;
}

std::string option_values_t::format_values()
{
    std::string result;
    if (values.empty()) {
        result = "Unknown key";
        return result;
    }
    if (! values.empty()) {
        for (auto it = values.rbegin(); it != values.rend(); ++it) {
            result += fmt::format("* {} in file {}:\n",
                                  it == values.rbegin() ? "Defined" : "Override",
                                  it->first->native());
            for (const auto& [config, value] : it->second) {
                result.append(fmt::format("  * {}{} = {}\n",
                                          name,
                                          config.empty() ? "" : fmt::format("[{}]", config),
                                          value));
            }
            if (&*it != &values.front()) {
                result += "\n";
            }
        }
    }
    return result;
}

void option_values_t::add_value(const std::shared_ptr<std::filesystem::path>& filename,
                                const std::string& config, const std::string& value)
{
    for (auto& [exist_filename, config_values] : values) {
        if (exist_filename == filename) {
            config_values[config] = value;
            return;
        }
    }
    values.push_front({ filename, { { config, value } } });
}

const std::string& option_values_t::value(const std::string& config) const
{
    for (auto& [filename, config_values] : values) {
        std::string key = config;
        while (true) {
            const auto it = config_values.find(key);
            if (it != config_values.end()) {
                return config_values.at(key);
            }
            if (key.empty()) {
                break;
            }
            size_t pos = key.find_last_of("_");
            if (pos == std::string::npos) {
                key.erase();
            } else {
                key.erase(pos);
            }
        }
    }
    throw std::out_of_range("Not found");
}

CFG_END_NAMESPACE
