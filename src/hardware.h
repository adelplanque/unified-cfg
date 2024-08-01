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
#ifndef __hardware_h__
#define __hardware_h__

#include "core.h"

CFG_BEGIN_NAMESPACE

/**
 * Return number of cpu cores taken from /proc/cpuinfo
 *
 * @param logical Give number of logical cores
 * @param min Minimal return value
 * @return Number of cpu cores
 */
int cpu_cores(bool logical, int min);

/**
 * Return value from /proc/meminfo
 *
 * @param key Value from /proc/meminfo to be retreive
 * @return Number of cpu cores
 */
long int meminfo(const std::string& key);

CFG_END_NAMESPACE

#endif
