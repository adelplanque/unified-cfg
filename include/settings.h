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

#ifndef __settings_h__
#define __settings_h__

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "core.h"
#include "value.h"

CFG_BEGIN_NAMESPACE

/** Cast string as type T */
template<typename T> T cast_value(const std::string value)
{
    return boost::lexical_cast<T>(value);
}
/** Cast string as boolean */
template<> bool cast_value<bool>(const std::string value);


class value_t
{
public:
    value_t(const std::string& value)
        : value(value)
    {}
    value_t(const std::string& value, option_values_t&& config_values)
        : value(value)
        , config_values(std::move(config_values))
    {}
    value_t(value_t&& other)
        : value(std::move(other.value))
        , config_values(std::move(other.config_values))
    {}

    /** Cast value as type T */
    template<typename T> T as() { return cast_value<T>(value); }

    std::string& get() { return this->value; }
    std::string format_values() { return config_values.format_values(); }

private:
    std::string value;
    option_values_t config_values;
};


/**
 * Node type for the configuration tree, node can be a value (string)
 * or a mapping of key and sub-node.
 */
class settings_t : public std::enable_shared_from_this<settings_t>
{
public:
    typedef std::shared_ptr<settings_t> ptr;
    typedef std::map<std::string, ptr> mapping_t;
    typedef std::optional<mapping_t::iterator> iterator;

    struct lazy_directory_t
    {
        lazy_directory_t(std::filesystem::path&& path) : path_(std::move(path)) {};
        std::filesystem::path path_;
    };
    struct lazy_file_t {
        lazy_file_t(std::filesystem::path&& path) : path_(std::move(path)) {};
        std::filesystem::path path_;
    };

    /** Content type for this node, can be mapping_t or value_t */
    typedef std::variant<mapping_t, value_t, lazy_directory_t, lazy_file_t> content_t;

    struct is_lazy
    {
        bool operator()(const mapping_t&) { return false; };
        bool operator()(const value_t&) { return false; };
        bool operator()(const lazy_directory_t&) { return true; };
        bool operator()(const lazy_file_t&) { return true; };
    };

    struct lazy_resolve
    {
        void operator()(const mapping_t&) {};
        void operator()(const value_t&) {};
        void operator()(const lazy_directory_t&);
        void operator()(const lazy_file_t&);
        settings_t& settings_;
    };

    struct is_value_visitor
    {
        bool operator()(const mapping_t&) { return false; };
        bool operator()(const value_t&) { return true; };
        bool operator()(const lazy_directory_t&) { return false; };
        bool operator()(const lazy_file_t&) { return false; };
    };

public:
    settings_t()
        : parent_(nullptr)
        , content_(lazy_directory_t{"."})
    {};
    template<typename T>
    settings_t(const std::string& name, T&& content, ptr parent=nullptr)
        : name_(name)
        , parent_(parent)
        , content_(std::move(content))
    {};

    ptr at(const std::string& key);
    void append(const std::string& key, ptr& settings);
    size_t count(const std::string& key);

    /** Remove content of this setting */
    void clear();

    /** List of keys contained in this settings node */
    std::vector<std::string> keys();

    /**
     * Build documentation for this configuration key
     *
     * @param os Output stream where documentation shoud be written
     */
    void doc(std::ostream& os);

    /** Cast node value as type T */
    template<typename T>
    T as()
    {
        if (std::holds_alternative<value_t>(content_)) {
            return std::get<value_t>(content_).as<T>();
        }
        throw std::out_of_range("Key error: " + full_name());
    }

    /** Check if this node is a leaf */
    bool is_value() { return std::visit(is_value_visitor{}, content_); }

    /**
     * Check content type for this setting
     *
     * @param T Type checked
     * @return True if content is of type T
     */
    template<typename T> bool is() { return std::holds_alternative<T>(this->content_); }

    size_t size();
    std::string full_name();
    std::out_of_range out_of_range(const std::string& key);
    std::filesystem::path get_path();
    ptr get_root()
    {
        return (this->parent_ == nullptr) ? shared_from_this() : this->parent_->get_root();
    }
    void set_comments(comments_t&& comments) { this->comments = std::move(comments); }

    iterator begin()
    {
        ensure_not_lazy();
        if (std::holds_alternative<mapping_t>(this->content_)) {
            return std::get<mapping_t>(this->content_).begin();
        } else {
            return std::nullopt;
        }
    }

    iterator end()
    {
        ensure_not_lazy();
        if (std::holds_alternative<mapping_t>(this->content_)) {
            return std::get<mapping_t>(this->content_).end();
        } else {
            return std::nullopt;
        }
    }

private:

    /** Ensures content is fully charged */
    void ensure_not_lazy();

    std::string name_;
    ptr parent_;
    content_t content_;
    comments_t comments;
};

CFG_END_NAMESPACE

#endif
