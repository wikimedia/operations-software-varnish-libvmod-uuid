CONTRIBUTING
============

To contribute code or documentation, submit a pull request at the
`source repository website
<https://github.com/otto-de/libvmod-uuid>`_.

If you have a problem or discover a bug, you can write to
<varnish-support@uplex.de>.

For developers
--------------

The build specifies C99 conformance for the C source (``-std=c99``),
with all compiler warnings turned on, and all warnings considered
errors (``-Werror -Wall -Wextra``). Compilation has been tested with
gcc and clang. The code MUST always build without warnings or errors
under these constraints with both compilers.

By default, ``CFLAGS`` are set to ``-g -O2``, so that symbols are
included in the shared library, and optimization is at level
``O2``. To change or disable these options, set ``CFLAGS`` explicitly
before calling ``configure`` (may be set to the empty string).

For development/debugging cycles, the ``configure`` option
``--enable-debugging`` is recommended (off by default). This will turn
off optimizations and function inlining, so that a debugger will step
through the code as expected.

By default, the VMOD is built with the stack protector enabled
(compile option ``-fstack-protector``), but it can be disabled with
the ``configure`` option ``--disable-stack-protector``.
