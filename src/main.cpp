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

#include <boost/algorithm/string.hpp>

#include <CLI/CLI.hpp>
#include <fmt/format.h>

#include "settings.h"
#include "config.h"
#include "render.h"

using namespace cfg;

std::string get_key(const std::string key)
{
    settings_t::ptr settings(new settings_t());
    settings_t::ptr node = settings;
    std::vector<std::string> toks {};
    for (const auto& tok: boost::split(toks, key, boost::is_any_of("."))) {
        node = node->at(tok);
    }
    return node->as<std::string>();
}

void doc_key(const std::string& key)
{
    settings_t::ptr settings(new settings_t());
    settings_t::ptr node = settings;
    std::vector<std::string> toks {};
    for (const auto& tok: boost::split(toks, key, boost::is_any_of("."))) {
        node = node->at(tok);
    }
    node->doc(std::cout);
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


void list_callback() {
    settings_t::ptr settings(new settings_t());
    for (settings_t::rec_iterator it = settings->rec_begin();
         it != settings->rec_end(); ++it) {
        std::cout << it->full_name() << ": " << it->as<std::string>() << std::endl;
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
        try {
            std::cout << get_key(key) << std::endl;
        }
        catch (std::out_of_range& e) {
            throw std::runtime_error(e.what());
        }
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

    auto list_subcommand = app.add_subcommand("list", "List keys/values");
    list_subcommand->final_callback(list_callback);

    try {
        CLI11_PARSE(app, argc, argv);
    }
    catch (std::runtime_error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
