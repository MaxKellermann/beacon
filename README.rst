Beacon
======

Beacon is a light-weight UDP-based protocol for sharing GPS fixes.
It was designed to locate friends during bike rides.

This repository contains both the protocol definition and a server
implementation.

This is work in progress.


Building
--------

You need:

- a C++20 compliant compiler (e.g. gcc or clang)
- `Boost <http://www.boost.org/>`__
- `PostgreSQL <https://www.postgresql.org/>`__ and `PostGIS <https://postgis.net/>`__
- `libfcgi <https://github.com/FastCGI-Archives>`__
- `jsoncpp <https://github.com/open-source-parsers/jsoncpp>`__
- `Meson 0.47 <http://mesonbuild.com/>`__ and `Ninja <https://ninja-build.org/>`__

Optional:

- `systemd <https://www.freedesktop.org/wiki/Software/systemd/>`__

Run ``meson``::

 meson . output

Compile and install::

 ninja -C output
 ninja -C output install
