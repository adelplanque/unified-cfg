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

#ifndef __cfg_h__
#define __cfg_h__

#include "core.h"
#include "settings.h"

CFG_BEGIN_NAMESPACE

/**
 * Render a model from stream
 *
 * @param settings_t Configuration to use for rendering
 * @param Template data stream
 * @return Completed data character string
 */
std::string render(settings_t::ptr, std::istream&);


/**
 * Render a model from string
 *
 * @param settings_t Configuration to use for rendering
 * @param Template data string
 * @return Completed data character string
 */
std::string render(settings_t::ptr, const std::string&);

CFG_END_NAMESPACE

#endif
