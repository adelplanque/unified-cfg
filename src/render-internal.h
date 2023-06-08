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

#ifndef __render_h__
#define __render_h__

#include <istream>
#include <list>
#include <optional>
#include <string>

#include <jinja2cpp/template.h>

#include "core.h"
#include "render.h"
#include "settings.h"

CFG_BEGIN_NAMESPACE

/**
 * Manage model rendering
 */
class Renderer
{
public:
    /** Constructor
     * @param settings_t Configuration to use for rendering
     * @param bool Recursive rendering */
    Renderer(settings_t::ptr settings, bool recursive=false);

    /** Render a data stream
     * @param Data stream */
    std::string render(std::istream& is);

    /** Render a string
     * @param Character string to be rendered */
    std::string render(std::string& str);

    /** Push a search key onto the stack
     * @return Error message when a circular search is detected */
    std::optional<std::string> push_back(const std::string&& key);

    /** Delete last search key */
    void pop() { this->stack.pop_back(); }

    /** Flag when rendering should be recursive */
    bool recursive;

private:
    jinja2::ValuesMap values;
    std::list<std::string> stack;
};

CFG_END_NAMESPACE

#endif
