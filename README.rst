============
vmod_uuid
============

----------------------
Varnish UUID Module
----------------------

:Author: Mitchell Broome
:Date: 2013-08-01
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
prepackaged library from your linux distribution.  i

In the case of Redhat/Fedora/CentOS, the rpm is named uuid.  Ensure
you install the rpms with the following command::

   yum install -y uuid uuid-devel


INSTALLATION
============

This is a basic implementation to generate a uuid for use in varnish.

The source tree is based on autotools to configure the building, and
does also have the necessary bits in place to do functional unit tests
using the varnishtest tool.

Usage::

 ./autogen.sh
 ./configure VARNISHSRC=DIR [VMODDIR=DIR]

`VARNISHSRC` is the directory of the Varnish source tree for which to
compile your vmod. Both the `VARNISHSRC` and `VARNISHSRC/include`
will be added to the include search paths for your module.

Optionally you can also set the vmod install directory by adding
`VMODDIR=DIR` (defaults to the pkg-config discovered directory from your
Varnish installation).

Make targets:

* make - builds the vmod
* make install - installs your vmod in `VMODDIR`
* make check - runs the unit tests in ``src/tests/*.vtc``

In your VCL you could then use this vmod along the following lines::
        
        import uuid;

        sub vcl_recv {
                # This sets req.http.X-Flow-ID to "cache-uuid"
                set req.http.X-Flow-ID = "cache-" + uuid.uuid();
        }


COPYRIGHT
=========

This document is licensed under the same license as the
libvmod-uuid project. See LICENSE for details.

