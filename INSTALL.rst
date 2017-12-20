INSTALLATION
============

RPMs
~~~~

Binary, debuginfo and source RPMs for VMOD re are available at
packagecloud:

	https://packagecloud.io/uplex/varnish

The packages are built for Enterprise Linux 7 (el7), and hence will
run on compatible distros (such as RHEL7, Fedora, CentOS 7 and Amazon
Linux).

To set up your YUM repository for the RPMs, follow these instructions:

	https://packagecloud.io/uplex/varnish/install#manual-rpm

You will also need these additional repositories:

* EPEL7

  * ``yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm``

* Official Varnish packages from packagecloud (since version 5.2.0)

  * Follow the instructions at: https://packagecloud.io/varnishcache/varnish52/install#manual-rpm

If you have problems or questions concerning the RPMs, contact
<varnish-support@uplex.de>.

Building from source
~~~~~~~~~~~~~~~~~~~~

The VMOD is built against a Varnish installation, and the autotools
use ``pkg-config(1)`` to locate the necessary header files and other
resources for Varnish. The development version of the OSSP uuid
library must also be installed, typically provided as a package named
uuid-devel.

This sequence will install the VMOD::

  > ./autogen.sh	# for builds from the git repo
  > ./configure
  > make
  > make check		# to run unit tests in src/tests/*.vtc
  > sudo make install

See `CONTRIBUTING.rst <CONTRIBUTING.rst>`_ for more detailed
information for developers.

If you have installed Varnish in a non-standard directory, call
``autogen.sh`` and ``configure`` with the ``PKG_CONFIG_PATH``
environment variable set to include the paths where ``varnishapi.pc``
can be located. For example, when varnishd configure was called with
``--prefix=$PREFIX``, use::

  > PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
  > export PKG_CONFIG_PATH

By default, the vmod ``configure`` script installs the vmod in the
same directory as Varnish, determined via ``pkg-config(1)``. The vmod
installation directory can be overridden by passing the ``VMOD_DIR``
variable to ``configure``.

Other files such as the man-page are installed in the locations
determined by ``configure``, which inherits its default ``--prefix``
setting from Varnish.
