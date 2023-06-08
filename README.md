What is it ?
============

`unified-cfg` allows unified management of different configuration
files. It is possible to decline the configuration in different
variants (DEV, OPER, ...), and to locally overload a default
configuration.

The API allows accessing each configuration key individually, or using
the configuration to complete jinja-like templates.

Each configuration file is an `.ini` file, this venerable format being
by its simplicity well suited to the administrators of an instance.

The different variants are indicated in square brackets at the end of
the key name.

```ini
[group]
key1[DEV] = value for dev env
key1[OPER] = value for oper env
```

The keys are searched in the different folders defined by the
`CONFIG_PATH` variable. The value returned is that of the first folder
to resolve the key according to the `CONFIG_NAME` variant.


Example
=======

Suppose 2 folders `/default` and `/local` containing a file
`my_app.ini`.

```ini
# /default/my_app.ini
[group]

key1 = default value 1
key1[OPER] = default oper key1 value

key2 = default value 2
key2[OPER] = default oper key2 value
```

```ini
# /local/my_app.ini
[group]
key2[OPER_NODE1] = local node1 value
```

Assuming:

* `CONFIG_PATH=/local:/default`
* `CONFIG_NAME=OPER_NODE1`

We have:

```bash
$ cfg get my_app.group.key1
default oper key1 value
$ cfg get my_app.group.key2
local node1 value
```


Commands
========

* `cfg get [key]`: Return the value of a key
* `cfg render [template]`: Formats the template with
  configuration values using jinja2 syntax.
* `cfg doc [key]`: Shows the documentation of the key as
  well as its various value overloads.


Use case
========

Consider a web server with plugins providing optional services. In the
configuration of nginx, we need to declare backends for each plugin
and map URLs to those backends.

* Main application, installed in `/path/to/application`:
  ```ini
  # /path/to/application/config/application.ini
  [nginx]
  port = 8080
  # ...
  ```

* First plugin, installed in `/path/to/first/plugin`:
  ```ini
  # /path/to/first/plugin/config/plugins/first_plugin.ini
  [main]
  enabled = true
  name = my_first_plugin
  [nginx]
  socket = /var/first_plugin.sock
  urls = ^/
  ```

* Maybe other plugins are installed in their own directory.

* The manager of a local instance overrides the configuration in a separate directory:
  ```ini
  # /path/to/local/config/plugins/first_plugin.ini
  [main]
  # To deactivate this plugin on this instance
  enabled = false
  ```

* And the nginx configuration
  ```
  http {
      listen {{settings.application.nginx.port}};

      {% for plugin in settings.plugins %}
      {% if plugin.main.enabled %}
      upstream {{plugin.main.name}} {
          server unix:{{plugin.nginx.socket}};
      }
      {% endif %}
      {% endfor %}

      server {
          {% for plugin in settings.plugins %}
          {% if plugin.main.enabled %}
          {% for url in plugin.nginx.urls.split() %}
          location ~ {{url}} {
              proxy_pass http://{{plugin.main.name}};
          }
          {% endfor %}
          {% endif %}
          {% endfor %}
      }
    }

We can render the nginx configuration with:

```bash
cfg -p /path/to/local/config -p /path/to/first/plugin/config \
    -p /path/to/application/config render nginx.conf
```

Or:

```bash
export CONFIG_PATH=/path/to/local/config:/path/to/first/plugin/config:/path/to/application/config
cfg render nginx.conf
```


Other features
==============

Shell call
----------

It is possible to call system commands an insert stdout in templates:

`{{shell("commande")}}`

Récursive key definition
------------------------

It is allowed to write

```ini
# first.ini
[main]
key1 = value
```

```ini
# second.ini
[main]
key1 = {{settings.first.main.key1}}
key2 = {{settings.second.main.key1}}
```


Configuration documentation
===========================


Installation
============

Requirements
------------

Building
--------


License
=======

unified-cfg is under LGPL-3.0-or-later.

unified-cfg embded various third-parties softwares:

* [Jinja2CPP](https://github.com/jinja2cpp/Jinja2Cpp): Mozilla Public License Version 2.0
* [fmt](https://github.com/fmtlib/fmt): MIT License
* [CLI11](https://github.com/CLIUtils/CLI11): BSD-3-Clause
* [subprocess](https://github.com/benman64/subprocess): MIT License
* [expected-lite](https://github.com/martinmoene/expected-lite): BSL-1.0
* [optional-lite](https://github.com/martinmoene/optional-lite): BSL-1.0
* [string-view-lite](https://github.com/martinmoene/string-view-lite): BSL-1.0
* [variant-lite](https://github.com/martinmoene/variant-lite): BSL-1.0
* [rapidjson](https://github.com/Tencent/rapidjson): MIT License
