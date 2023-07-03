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

#include <exception>
#include <fstream>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h> // for STDOUT_FILENO

#include <boost/algorithm/string.hpp>

#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <tabulate/table.hpp>

#include "settings.h"
#include "config.h"
#include "render.h"

using namespace cfg;

settings_t::ptr get_node(const settings_t::ptr& settings, const std::string& key)
{
    settings_t::ptr node = settings;
    if (! key.empty()) {
        std::vector<std::string> toks {};
        for (const auto& tok: boost::split(toks, key, boost::is_any_of("."))) {
            node = node->at(tok);
        }
    }
    return node;
}

std::string get_key(const std::string& key)
{
    settings_t::ptr settings(new settings_t());
    return get_node(settings, key)->as<std::string>();
}

void doc_key(const std::string& key)
{
    settings_t::ptr settings(new settings_t());
    get_node(settings, key)->doc(std::cout);
}

void append_config_paths(const std::vector<std::string>& paths) {
    for (const auto& path : paths) {
        std::cerr << "path: " << path << std::endl;
        config_t::get_instance().push_front_config_path(path);
    }
}


/**
 * Implementation of render command
 *
 * @param filename Template filename to be rendered
 */
void render_callback(const std::string& input, const std::string& output)
{
    std::string result;
    try {
        if (input.empty()) {
            result = render(std::make_shared<settings_t>(), std::cin);
        } else {
            std::ifstream is { input };
            result = render(std::make_shared<settings_t>(), is);
        }
    }
    catch (std::runtime_error& e)
    {
        std::string msg { e.what() };
        boost::algorithm::replace_all(msg, "noname.j2tpl", input);
        std::cerr << msg << std::endl;
    }

    if (output.empty()) {
        std::cout << result;
    } else {
        std::ofstream os { output };
        os << result;
    }
}


size_t wraps_key(std::string& key, size_t max)
{
    std::vector<std::string> toks;
    boost::split(toks, key, boost::is_any_of("."));
    std::string result = toks[0];
    size_t current = result.size();
    size_t total = 0;
    for (auto tok = ++toks.begin(); tok != toks.end(); ++tok) {
        if (current + tok->size() + 1 > max) {
            result += "\n";
            current = 0;
        }
        result += "." + *tok;
        current += tok->size() + 1;
        total = std::max(total, current);
    }
    std::swap(key, result);
    return total + 2;
}


void list_callback(const std::string& key, bool tabulate, std::optional<size_t> requested_width)
{
    settings_t::ptr settings(new settings_t());
    auto node = get_node(settings, key);
    if (tabulate) {
        size_t width;
        if (requested_width.has_value()) {
            width = requested_width.value();
        } else if (isatty(STDOUT_FILENO)) {
            struct winsize size;
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
            width = size.ws_col;
        } else {
            width = 80;
        }
        size_t key_width = 0;
        tabulate::Table table;
        table.format().multi_byte_characters(true);
        table.add_row({"Key", "Value"});
        table[0].format().font_style({tabulate::FontStyle::bold});
        for (settings_t::rec_iterator it = node->rec_begin();
             it != node->rec_end(); ++it) {
            std::string key = it->full_name();
            key_width = std::max(key_width, wraps_key(key, 40));
            table.add_row({key, it->as<std::string>()});
        }
        size_t value_width = std::max(width - key_width - 3, (size_t) 20);
        for (auto& row : table) {
            row[0].format().width(key_width);
            row[1].format().width(value_width);
        }
        std::cout << table << std::endl;
    } else {
        for (settings_t::rec_iterator it = node->rec_begin();
             it != node->rec_end(); ++it) {
            std::cout << it->full_name() << ": " << it->as<std::string>() << std::endl;
        }
    }
}


int main(int argc, char** argv)
{
    auto& config = config_t::get_instance();
    CLI::App app{"Manage configuration files"};

    std::string key;
    std::string template_filename, result_filename;

    app.add_option_function("--path",
                            std::function<void(const std::vector<std::string>&)>(append_config_paths),
                            "Path to configuration files");
    app.add_option("--config-name", config.get_config_name(),
                   fmt::format("Configuration name (default: {})", config.get_config_name()));

    auto get_subcommand = app.add_subcommand("get", "Get config key");
    get_subcommand->add_option("key", key, "Required key");
    get_subcommand->final_callback([&key]() {
        std::cout << get_key(key) << std::endl;
    });

    auto render_subcommand = app.add_subcommand("render", "Render template file");
    render_subcommand->add_option("input", template_filename, "Template file")->required(false);
    render_subcommand->add_option("output", result_filename, "Resulting file")->required(false);
    render_subcommand->final_callback([&template_filename, &result_filename]() {
        render_callback(template_filename, result_filename);
    });

    auto help_subcommand = app.add_subcommand("doc", "Documentation for given key");
    help_subcommand->add_option("key", key, "Required key");
    help_subcommand->final_callback([&key]() { doc_key(key); });

    bool tabulate = false;
    std::optional<size_t> width = std::nullopt;

    auto list_subcommand =
        app.add_subcommand("list", "List of configuration keys with their values.");
    auto tabulate_flag =
        list_subcommand->add_flag("--tabulate", tabulate, "Display results in tabular form");
    list_subcommand->add_option("--width", width, "Table width")->needs(tabulate_flag);
    list_subcommand->add_option("key", key, "Filter keys starting with this prefix")->required(false);
    list_subcommand->final_callback([&tabulate, &key, &width]() {
        list_callback(key, tabulate, width);
    });

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError &e) {
        return app.exit(e);
    }
    catch (std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
