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

#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <fmt/format.h>
#include <list>
#include <sstream>

#include <jinja2cpp/reflected_value.h>
#include <jinja2cpp/string_helpers.h>
#include <jinja2cpp/template.h>
#include <jinja2cpp/user_callable.h>
#include <subprocess.hpp>

#include "core.h"
#include "env.h"
#include "env_reflector.h"
#include "hardware.h"
#include "render-internal.h"
#include "settings.h"


namespace jinja2
{
namespace detail
{
    using namespace cfg;

    class SettingsMapAccessor : public IMapItemAccessor
                              , public ReflectedDataHolder<settings_t::ptr>
    {
    public:
        SettingsMapAccessor(settings_t::ptr val, Renderer& r)
            : ReflectedDataHolder<settings_t::ptr>::ReflectedDataHolder(val)
            , renderer(r)
        {};
        ~SettingsMapAccessor() override = default;

        bool IsEqual(const IComparable& other) const override
        {
            auto* val = dynamic_cast<const SettingsMapAccessor*>(&other);
            if (!val)
                return false;
            return *this->GetValue() == *val->GetValue();
        }

        size_t GetSize() const override
        { return (*this->GetValue())->size(); }

        bool HasValue(const std::string& name) const override
        { return (*this->GetValue())->count(name); }

        Value GetValueByName(const std::string& name) const override
        {
            auto settings = (*this->GetValue())->at(name);
            if (settings->is_value()) {
                auto value = settings->as<std::string>();
                if (renderer.recursive
                    && (value.find("{{") != std::string::npos
                        || value.find("{%") != std::string::npos)) {
                    auto error = renderer.push_back(settings->full_name());
                    if (error.has_value()) {
                        return error.value();
                    }
                    value = renderer.render(value);
                    renderer.pop();
                }
                return value;
            } else {
                return GenericMap([accessor = SettingsMapAccessor(settings, renderer)]() {
                    return &accessor;
                });
            }
        }

        std::vector<std::string> GetKeys() const override
        { return (*this->GetValue())->keys(); }

    private:
        Renderer& renderer;
    };
}
}


CFG_BEGIN_NAMESPACE

/**
 * Implement shell function in templates.
 *
 * @param script Script passed to shell interpreteur
 * @param ignore_error Don't return error message when shell return non 0 code
 * @return Shell stdout
 */
std::string shell(const std::string& script, bool ignore_error=false)
{
    std::string result;
    const char* shell_p = std::getenv("SHELL");
    try {
        auto process =
            subprocess::run({shell_p ? shell_p : "sh", "-c", script},
                            subprocess::RunBuilder()
                            .cout(subprocess::PipeOption::pipe)
                            .cerr(subprocess::PipeOption::pipe)
                            .check(true));
        result = std::move(process.cout);
        boost::algorithm::trim(result);
    }
    catch (const subprocess::CalledProcessError& e) {
        if (ignore_error) {
            result = std::move(e.cout);
            boost::algorithm::trim(result);
        } else {
            result = fmt::format("{{shell error [{}]: {}}}",
                                 e.returncode, boost::algorithm::trim_copy(e.cerr));
        }
    }
    return result;
}


Renderer::Renderer(settings_t::ptr settings, bool recursive)
    : recursive(recursive)
{
    values = jinja2::ValuesMap({
            {"env", jinja2::Reflect(environnement_t())},
            {"settings", jinja2::GenericMap([accessor = jinja2::detail::SettingsMapAccessor(settings, *this)]() {
                return &accessor;
            })},
            {"shell", jinja2::MakeCallable(shell,
                                           jinja2::ArgInfo("script", true),
                                           jinja2::ArgInfo("ignore_error", false, false))},
            {"hardware", jinja2::ValuesMap({
                        {"cpu_cores", jinja2::MakeCallable(cpu_cores)},
                        {"meminfo", jinja2::MakeCallable(meminfo, jinja2::ArgInfo("key", true))},
                    })},
        });
}


std::string Renderer::render(std::istream& is)
{
    jinja2::Template tpl;
    tpl.Load(is).value();
    return tpl.RenderAsString(this->values).value();
}


std::string Renderer::render(std::string& str)
{
    auto is = std::istringstream(str);
    auto res = this->render(is);
    return res;
}


std::optional<std::string> Renderer::push_back(const std::string&& key)
{
    if (std::find(this->stack.begin(), this->stack.end(), key) != this->stack.end()) {
        std::string error = fmt::format("<Circular lookup: {} => {}>",
                                        boost::algorithm::join(this->stack, " => "), key);
        return error;
    }
    this->stack.push_back(std::move(key));
    return std::nullopt;
}


std::string render(settings_t::ptr settings, std::istream& is)
{
    try {
        return Renderer(settings).render(is);
    }
    catch (nonstd::expected_lite::bad_expected_access<jinja2::ErrorInfo>& e)
    {
        throw std::runtime_error(fmt::format("Template error at {}", e.error().ToString()));
    }
}


std::string render(settings_t::ptr settings, const std::string& data)
{
    std::istringstream is { data };
    return render(settings, is);
}

CFG_END_NAMESPACE
