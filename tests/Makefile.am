TESTS = test-api-tagsOpen
check_PROGRAMS = test-api-tagsOpen

EXTRA_DIST =

AM_CPPFLAGS = -I $(top_builddir)
DEPS = $(top_builddir)/libreadtags.la
LDADD = $(top_builddir)/libreadtags.la

test_api_tagsOpen = test-api-tagsOpen.c
test_api_tagsOpen_DEPENDENCIES = $(DEPS)
EXTRA_DIST += api-tagsOpen.tags
