TESTS = \
	test-simple.sh
	$(NULL)

AM_TESTS_ENVIRONMENT = \
	DICTDIR=$(top_builddir)/data; \
	SWATH=$(top_builddir)/src/swath; \
	export DICTDIR SWATH; \
	$(NULL)

check_SCRIPTS = \
	test-simple.sh
	$(NULL)

EXTRA_DIST = \
	$(check_SCRIPTS) \
	$(NULL)
