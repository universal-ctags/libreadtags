TESTS = \
	\
	test-api-tagsOpen \
	test-api-tagsFind \
	\
	$(NULL)

check_PROGRAMS = \
	\
	test-api-tagsOpen \
	test-api-tagsFind \
	\
	$(NULL)

EXTRA_DIST =

AM_CPPFLAGS = -I $(top_srcdir)
DEPS = $(top_builddir)/libreadtags.la
LDADD = $(top_builddir)/libreadtags.la

test_api_tagsOpen = test-api-tagsOpen.c
test_api_tagsOpen_DEPENDENCIES = $(DEPS)
EXTRA_DIST += api-tagsOpen.tags

test_api_tagsFind = test-api-tagsFind.c
test_api_tagsFind_DEPENDENCIES = $(DEPS)
EXTRA_DIST += duplicated-names.c
EXTRA_DIST += duplicated-names--sorted-yes.tags
EXTRA_DIST += duplicated-names--sorted-no.tags
EXTRA_DIST += duplicated-names--sorted-foldcase.tags
