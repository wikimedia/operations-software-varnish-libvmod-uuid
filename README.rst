============
vmod_uuid
============

----------------------
Varnish UUID Module
----------------------

:Author: Mitchell Broome, Geoff Simmons
:Date: 2014-12-11
:Version: 1.0
:Manual section: 3

SYNOPSIS
========

import uuid;

DESCRIPTION
===========

UUID Varnish vmod used to generate a uuid, including versions 1, 3, 4
and 5 as specified in RFC 4122. See the RFC for details about the
various versions.


FUNCTIONS
=========

uuid
-----

Prototype
        ::

                uuid()
Return value
	STRING
Description
	Returns a uuid version 1 (based on MAC address and the current time)
UUID
        ::

                set req.http.X-Flow-ID = "cache-" + uuid.uuid();

uuid_v1
-------

Prototype
        ::

                uuid_v1()
Return value
	STRING
Description
	Returns a uuid version 1. The functions `uuid()` and `uuid_v1()`
        are aliases for one another.
Example
        ::

                set req.http.X-Flow-ID = "cache-" + uuid.uuid_v1();

uuid_v3
-------

Prototype
        ::

                uuid_v3(STRING namespace, STRING name)
Return value
	STRING
Description
	Returns a uuid version 3, based on an MD5 hash formed from
        `namespace` and `name`. The `namespace` argument MUST be
        one of the following:

* "nil" (for the "nil UUID")
* "ns:DNS" (for the domain name system)
* "ns:URL" (for the URL namespace)
* "ns:OID" (for the ISO object identifier namespace)
* "ns:X500" (for X.500 distinguished names)
* a valid 36-character representation of a UUID, i.e. a string of the form "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx", where all of the x's are hex digits, with appropriate restrictions on UUID formats (cf. the RFC)

If these conditions are not met, then `uuid_v3()` fails; it returns
a null string (typically, the header specified in VCL will not be set),
and an error message is emitted to the Varnish log with the tag
`VCL_error`.

The `name` argument can be any string.

Example
        ::

              set req.http.X-DNS-ID = uuid.uuid_v3("ns:DNS", "www.widgets.com");
              set req.http.X-MyNS-ID = uuid.uuid_v3("6ba7b810-9dad-11d1-80b4-00c04fd430c8", "www.widgets.com");

uuid_v4
-------

Prototype
        ::

                uuid_v4()
Return value
	STRING
Description
	Returns a uuid version 4, based on random numbers.
Example
        ::

                set req.http.X-Rand-ID = uuid.uuid_v4();

uuid_v5
-------

Prototype
        ::

                uuid_v5(STRING namespace, STRING name)
Return value
	STRING
Description
	Returns a uuid version 5, based on a SHA1 hash formed from
        `namespace` and `name`. The same restrictions and failure
        conditions regarding the `namespace` argument hold as for
        `uuid_v3()` above. The `name` argument can be any string.
Example
        ::

              set req.http.X-DNS-ID = uuid.uuid_v5("ns:DNS", "www.widgets.com");
              set req.http.X-MyNS-ID = uuid.uuid_v5("6ba7b810-9dad-11d1-80b4-00c04fd430c8", "www.widgets.com");

DEPENDENCIES
============

Libvmod-uuid requires the OSSP uuid library to generate uuids.  It
is available at http://www.ossp.org/pkg/lib/uuid/ or possibly as a
prepackaged library from your linux distribution.

In the case of Redhat/Fedora/CentOS, the rpm is named uuid.  Ensure
you install the rpms with the following command::

   yum install -y uuid uuid-devel


INSTALLATION
============

The installation process is standard for a Varnish 4 VMOD -- build the
VMOD on a system where an instance of Varnish 4 is installed, and the
auto-tools will attempt to locate the Varnish instance, and then pull
in libraries and other support files from there.

Quick start
-----------

This sequence should be enough in typical setups:

1. ``./autogen.sh``  (for git-installation)
2. ``./configure``
3. ``make``
4. ``make check`` (regression tests)
5. ``make install`` (may require root: sudo make install)

In your VCL you could then use this vmod along the following lines::
        
        import uuid;

        sub vcl_recv {
                # This sets req.http.X-Flow-ID to "cache-uuid"
                set req.http.X-Flow-ID = "cache-" + uuid.uuid();
        }

Alternative configs
-------------------

As with Varnish itself, you can set additional flags and macros in the
``configure`` step, and/or define environment variables to affect the
build config.

For example, if you are building the VMOD against a Varnish instance
with a non-standard installation prefix, then set these env variables
before running ``configure``:

* PREFIX=/path/to/varnish/install/prefix
* export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig
* export ACLOCAL_PATH=$PREFIX/share/aclocal
* export PATH=$PREFIX/bin:$PREFIX/sbin:$PATH

``configure`` must locate the ``varnishtest`` and ``varnishd``
binaries so that ``make check`` can be run. Usually it should be able
to find them, but if necessary you can set the variables
``VARNISHTEST`` and/or ``VARNISHD`` with the full paths.

For developers
--------------

As with Varnish, you can use these ``configure`` options for stricter
compiling:

* ``--enable-developer-warnings``
* ``--enable-extra-developer-warnings`` (for GCC 4)
* ``--enable-werror``

The VMOD must always build successfully with these options enabled.

Also as with Varnish, you can add ``--enable-debugging-symbols``, so
that the VMOD's symbols are available to debuggers, in core dumps and
so forth.

HISTORY
=======

* Version 1.0: Varnish 4 version, supporting all UUID variants
  by Geoffrey Simmons <geoff@uplex.de>, UPLEX Nils Goroll Systemoptimierung
  for Otto GmbH & KG
  https://github.com/otto-de/libvmod-uuid

* Version 0.1: Initial Varnish 3 version, by Mitchell Broome of Sharecare
  https://github.com/Sharecare/libvmod-uuid

COPYRIGHT
=========

This document is licensed under the same license as the
libvmod-uuid project. See LICENSE for details.

