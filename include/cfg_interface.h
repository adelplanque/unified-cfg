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

#ifndef __cfg_c_h__
#define __cfg_c_h__


struct cfg_renderer;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cfg_renderer* cfg_renderer_handler;
cfg_renderer_handler cfg_renderer_create();
int cfg_renderer_render_fd(cfg_renderer_handler, int);
int cfg_renderer_render_string(cfg_renderer_handler, char*);
const char *cfg_renderer_get_string(cfg_renderer_handler);
void cfg_renderer_free(cfg_renderer_handler);

#ifdef __cplusplus
}
#endif

#endif
