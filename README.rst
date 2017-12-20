============
vmod_uuid
============

----------------------
Varnish UUID Module
----------------------

:Author: Mitchell Broome, Geoff Simmons
:Date: 2017-10-30
:Version: 1.3
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

Libvmod-uuid requires the OSSP uuid library to generate uuids.  It is
available at http://www.ossp.org/pkg/lib/uuid/ or possibly as a
prepackaged library (usually named uuid) from your linux distribution.

This version of the VMOD requires Varnish since version 5.0.0, and has
been tested against versions through 5.2.0.

INSTALLATION
============

See `INSTALL.rst <INSTALL.rst>`_ in the source repository.

HISTORY
=======

* Version 1.4: add RPM packaging

* Version 1.3: compatible with Varnish 5.0.0 through 5.2.

* Verison 1.2: compatible with Varnish since 5.0.0

* Version 1.1: requires Varnish 4.1, creates internal UUID objects
  only once during a client or backend context and re-uses them for
  subsequent calls.

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

