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

#ifndef __config_h__
#define __config_h__

#include <list>
#include <filesystem>

#include "core.h"

CFG_BEGIN_NAMESPACE

/**
 * Manage configuration
 */
class config_t {

public:

    /** Get instance of configuration */
    static config_t& get_instance()
    {
        if (config_t::instance == nullptr) {
            config_t::instance = new config_t();
        }
        return *config_t::instance;
    }

    /**
     * Get config path locations.
     *
     * @return Reference to list of paths, highest priority first
     */
    const std::list<std::filesystem::path>& get_config_path() const { return config_path; }

    /** Return current configuration name */
    std::string& get_config_name() { return config_name; }

    /** Add configuration files location with hight priority */
    void push_front_config_path(const std::string& path);

    /** Add configuration files location with low priority */
    void push_back_config_path(const std::string& path);

    /** clear_config_path */
    void clear_config_path() { config_path.clear(); }

    /** Return list of configuration path */
    const std::list<std::filesystem::path>& get_config_path() { return config_path; }

private:
    config_t();
    config_t(const config_t&) = delete;
    config_t(const config_t&&) = delete;
    /** Single instance of the configuration */
    static config_t* instance;
    /** List of paths where configuration files are located. Highest priority first. */
    std::list<std::filesystem::path> config_path;
    /** Current configuration name */
    std::string config_name;
};

CFG_END_NAMESPACE

#endif
