TESTS = \
	test-simple.sh \
	test-long.sh \
	$(NULL)

AM_TESTS_ENVIRONMENT = \
	DICTDIR=$(top_builddir)/data; \
	SWATH=$(top_builddir)/src/swath; \
	export DICTDIR SWATH; \
	$(NULL)

check_SCRIPTS = \
	test-simple.sh \
	test-long.sh \
	$(NULL)

DISTCLEANFILES = \
	long-out.txt \
	$(NULL)

EXTRA_DIST = \
	$(check_SCRIPTS) \
	long.txt \
	long-wseg.txt \
	$(NULL)
