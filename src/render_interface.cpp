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

#include <ext/stdio_filebuf.h>
#include <memory>
#include <streambuf>

#include "cfg_interface.h"

#include "render-internal.h"
#include "settings.h"


using namespace cfg;


struct cfg_renderer {
    std::string result;
};

struct membuf : std::streambuf {
    membuf(char* begin, char* end) {
        this->setg(begin, begin, end);
    }
};

cfg_renderer_handler cfg_renderer_create() {
    return new cfg_renderer();
}

int cfg_renderer_render_fd(cfg_renderer_handler handler, int fd)
{
    __gnu_cxx::stdio_filebuf<char> filebuf(fd, std::ios::in);
    std::istream is(&filebuf);

    try {
        handler->result = render(std::make_shared<settings_t>(), is);
        return 0;
    }
    catch (std::exception&)
    {
        return 1;
    }
}

int cfg_renderer_render_string(cfg_renderer_handler handler, char *s)
{
    membuf buf(s, s + strlen(s));
    std::istream is(&buf);
    try {
        handler->result = render(std::make_shared<settings_t>(), is);
        return 0;
    }
    catch (std::exception&)
    {
        return 1;
    }
}

const char *cfg_renderer_get_string(cfg_renderer_handler handler)
{
    return handler->result.c_str();
}

void cfg_renderer_free(cfg_renderer_handler handler)
{
    delete handler;
}
