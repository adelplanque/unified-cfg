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

#include <list>
#include <optional>
#include <stdexcept>
#include <string>
#include <set>

#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <fmt/format.h>

#include "core.h"
#include "config.h"
#include "parser.h"
#include "render-internal.h"
#include "settings.h"

CFG_BEGIN_NAMESPACE

template<> bool cast_value<bool>(const std::string value)
{
    std::string lower = boost::algorithm::to_lower_copy(value);
    if (lower == "true") {
        return true;
    }
    if (lower == "false") {
        return false;
    }
    return boost::lexical_cast<int>(value);
}

void settings_t::lazy_resolve::operator()(const lazy_directory_t& original_content)
{
    auto path = original_content.path_;

    std::set<std::string> directories;
    std::set<std::string> files;

    for (const auto& config_path : config_t::get_instance().get_config_path()) {
        auto fullpath = config_path / path;
        if (! std::filesystem::is_directory(fullpath)) {
            continue;
        }
        for (const auto& entry : std::filesystem::directory_iterator(fullpath)) {
            if (entry.is_directory()) {
                const auto& dir_name = entry.path().filename().native();
                if (dir_name.find(".") == std::string::npos) {
                    directories.insert(std::move(dir_name));
                }
            } else if (entry.is_regular_file() && entry.path().extension() == ".ini") {
                const auto& filename = entry.path().stem().native();
                if (filename.find(".") == std::string::npos) {
                    files.insert(std::move(filename));
                }
            }
        }
    }

    settings_.content_.emplace<settings_t::mapping_t>();
    auto& map = std::get<settings_t::mapping_t>(settings_.content_);
    for (const auto& name : files) {
        map.emplace(name,
                    std::make_shared<settings_t>(name, lazy_file_t(path / (name + ".ini")),
                                                 settings_.shared_from_this()));
    }
    for (const auto& name : directories) {
        map.try_emplace(name,
                        std::make_shared<settings_t>(name, lazy_directory_t(path / name),
                                                     settings_.shared_from_this()));
    }
}

void settings_t::lazy_resolve::operator()(const lazy_file_t& original_content)
{
    auto filename = original_content.path_;

    parser_t parser;
    std::list<std::shared_ptr<std::filesystem::path>> filenames;

    for (const auto& config_path
             : boost::adaptors::reverse(config_t::get_instance().get_config_path())) {
        auto fullname = config_path / filename;
        if (std::filesystem::is_regular_file(fullname)) {
            parser.load_file(std::make_shared<std::filesystem::path>(std::filesystem::canonical(fullname)));
        }
    }

    settings_.content_.emplace<settings_t::mapping_t>();
    auto& map = std::get<settings_t::mapping_t>(settings_.content_);

    const auto& config_name = config_t::get_instance().get_config_name();
    for (auto& [group_name, group] : parser.get_options()) {
        auto& [group_comments, group_map] = group;
        auto group_settings = std::make_shared<settings_t>(group_name, mapping_t{},
                                                           settings_.shared_from_this());
        map.emplace(group_name, group_settings);
        group_settings->set_comments(std::move(group_comments));
        for (auto& [option_name, option] : group_map) {
            auto& [option_comments, option_values] = option;
            auto option_settings
                = std::make_shared<settings_t>
                (option_name,
                 value_t(option_values.value(config_name), std::move(option_values)),
                 group_settings
                 );
            group_settings->append(option_name, option_settings);
            option_settings->set_comments(std::move(option_comments));
        }
    }

    Renderer r { settings_.get_root(), true };
    for (auto& group : map) {
        if (! group.second->is<mapping_t>()) {
            continue;
        }
        for (auto& option : std::get<mapping_t>(group.second->content_)) {
            if (! option.second->is<value_t>()) {
                continue;
            }
            auto& value = std::get<value_t>(option.second->content_).get();
            if (value.find("{{") != std::string::npos || value.find("{%") != std::string::npos) {
                value = r.render(value);
            }
        }
    }
}

settings_t::ptr settings_t::at(const std::string& key)
{
    ensure_not_lazy();
    try {
        return std::get<mapping_t>(content_).at(key);
    }
    catch (const std::bad_variant_access&) {
        throw out_of_range(key);
    }
    catch (const std::out_of_range&) {
        throw out_of_range(key);
    }
}

size_t settings_t::size()
{
    ensure_not_lazy();
    if (this->is_value()) {
        return 0;
    } else {
        return std::get<mapping_t>(content_).size();
    }
}

std::string settings_t::full_name()
{
    std::string result = (parent_ == nullptr) ? "" : parent_->full_name();
    if (! result.empty()) {
        result.append(".");
    }
    result.append(name_);
    return result;
}

std::out_of_range settings_t::out_of_range(const std::string& key) {
    return std::out_of_range(fmt::format("Key {} undefined in .{}", key, full_name()));
}

std::filesystem::path settings_t::get_path()
{
    std::filesystem::path p =
        (parent_ == nullptr) ? std::filesystem::path() : parent_->get_path();
    if (! name_.empty()) {
        p /= name_;
    }
    return p;
}

void settings_t::append(const std::string& key, settings_t::ptr& settings) {
    try {
        mapping_t& mapping = std::get<mapping_t>(content_);
        mapping.emplace(key, settings);
    }
    catch (std::bad_variant_access&) {
        throw out_of_range(key);
    }
}

void settings_t::ensure_not_lazy()
{
    std::visit(settings_t::lazy_resolve{*this}, content_);
}

size_t settings_t::count(const std::string& key)
{
    ensure_not_lazy();
    try {
        return std::get<mapping_t>(content_).count(key);
    }
    catch (const std::bad_variant_access&) {
        return 0;
    }
}

void settings_t::clear()
{
    content_ = settings_t::lazy_directory_t(".");
}

std::vector<std::string> settings_t::keys()
{
    ensure_not_lazy();
    if (this->is_value()) {
        throw std::out_of_range(fmt::format("Key error: {} is a leaf object", full_name()));
    }
    auto mapping = std::get<mapping_t>(content_);
    std::vector<std::string> result;
    for (const auto& item : mapping) {
        result.push_back(item.first);
    }
    return result;
}

void settings_t::doc(std::ostream& os)
{
    if (this->is_value()) {
        std::string header = fmt::format("Option {}", this->full_name());
        os << header << std::endl
           << std::string(header.size(), '.') << std::endl << std::endl;
        if (this->comments.size() > 0) {
            os << this->comments.format() << std::endl;
        }
        os << std::get<value_t>(content_).format_values();
    } else {
        std::string header = fmt::format("Group {}", this->full_name());
        os << header << std::endl
           << std::string(header.size(), '-') << std::endl;
        if (this->comments.size() > 0) {
            os << std::endl << this->comments.format();
        }
        auto keys = this->keys();
        if (! keys.empty()) {
            os << std::endl;
            std::sort(keys.begin(), keys.end());
            for (const auto& key : keys) {
                this->at(key)->doc(os);
                if (&key != &keys.back()) {
                    os << std::endl;
                }
            }
        }
    }
}

CFG_END_NAMESPACE
