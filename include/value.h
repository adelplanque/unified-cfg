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

#ifndef __value_h__
#define __value_h__

#include <filesystem>
#include <list>
#include <memory>
#include <regex>
#include <string>

#include "core.h"

CFG_BEGIN_NAMESPACE

/**
 * Storing an option with multiple value variants
 */
class option_values_t
{
private:
    /** Value associated with a variant */
    typedef std::map<std::string, std::string> config_values_t;
    /** All the variants present in a file */
    typedef std::pair<std::shared_ptr<std::filesystem::path>, config_values_t> filename_values_t;
    /** List of variants for different files */
    typedef std::list<filename_values_t> values_t;

public:
    option_values_t() {};
    option_values_t(const std::string& name)
        : name(name)
    {};
    option_values_t(option_values_t&& other)
        : name(std::move(other.name))
        , values(std::move(other.values))
    {};

    void add_value(const std::shared_ptr<std::filesystem::path>& filename,
                   const std::string& config, const std::string& value);
    const std::string& value(const std::string& config) const;
    std::string format_values();

private:
    std::string name;
    values_t values;
};


/**
 * Storing a comment associated with an option.
 */
class comments_t {
public:
    comments_t() {}
    comments_t(comments_t&& other) : lines(std::move(other.lines)) {}
    comments_t& operator=(comments_t&& other)
    {
        this->lines = std::move(other.lines);
        return *this;
    };
    void append(std::list<std::string>&& lines);
    std::string format();
    size_t size() { return lines.size(); }

private:
    std::list<std::string> lines;
};

CFG_END_NAMESPACE

#endif
