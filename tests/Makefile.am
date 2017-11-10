TESTS = \
	test-simple.sh \
	test-long.sh \
	test-mixed.sh \
	test-utf8-wbr.sh \
	test-latex.sh \
	$(NULL)

AM_TESTS_ENVIRONMENT = \
	DICTDIR=$(top_builddir)/data; \
	SWATH=$(top_builddir)/src/swath; \
	export DICTDIR SWATH; \
	$(NULL)

check_SCRIPTS = \
	test-simple.sh \
	test-long.sh \
	test-mixed.sh \
	test-utf8-wbr.sh \
	test-latex.sh \
	$(NULL)

CLEANFILES = \
	long-out.txt \
	mixed-out.txt \
	thai-latex-out.tex \
	$(NULL)

EXTRA_DIST = \
	$(check_SCRIPTS) \
	long.txt \
	long-wseg.txt \
	mixed.txt \
	mixed-wseg.txt \
	thai-latex.tex \
	thai-latex-wseg.tex \
	$(NULL)
