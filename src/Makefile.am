AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = $(VARNISHAPI_CFLAGS) @OSSP_CFLAGS@ -DSTATIC_ASSERT -Wall -Werror -Wextra -std=c99
AM_LDFLAGS = $(VARNISHAPI_LIBS) $(VMOD_LDFLAGS) @OSSP_LIBS@

vmod_LTLIBRARIES = libvmod_uuid.la

libvmod_uuid_la_SOURCES = \
	vmod_uuid.c

nodist_libvmod_uuid_la_SOURCES = \
	vcc_if.c \
	vcc_if.h

vmod_uuid.lo: vcc_if.c vcc_if.h

vmod_uuid.lo: $(nodist_libvmod_uuid_la_SOURCES)

vcc_if.c: vcc_if.h

vcc_if.h: vmod_uuid.vcc
	$(AM_V_VMODTOOL) $(PYTHON) $(VMODTOOL) -o vcc_if $(srcdir)/vmod_uuid.vcc

AM_TESTS_ENVIRONMENT = \
	PATH="$(VMOD_TEST_PATH)" \
	LD_LIBRARY_PATH="$(VARNISH_LIBRARY_PATH)"
TEST_EXTENSIONS = .vtc
VTC_LOG_COMPILER = varnishtest -v
AM_VTC_LOG_FLAGS = -Dvmod_uuid="$(VMOD_UUID)"

TESTS = @VMOD_TESTS@

EXTRA_DIST = \
	vmod_uuid.vcc \
	$(VMOD_TESTS)

CLEANFILES = \
	$(builddir)/vcc_if.c \
	$(builddir)/vcc_if.h \
	$(builddir)/vmod_uuid.rst \
	$(builddir)/vmod_uuid.man.rst
