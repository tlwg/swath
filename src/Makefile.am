MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir) $(DATRIE_CFLAGS)
DEFS = -DWORDSEGDATA_DIR=\"${pkgdatadir}\" -DVERSION=\"$(VERSION)\"

EXTRA_DIST = \
	abswordseg.h \
	convutil.h \
	dict.h \
	filefilter.h \
	filterhtml.h \
	filterlambda.h \
	filterlatex.h \
	filterrtf.h \
	filterx.h \
	longwordseg.h \
	maxwordseg.h \
	utils.h \
	worddef.h \
	wordstack.h \
	swath.1

bin_PROGRAMS = swath

swath_SOURCES = \
	dict.cpp \
	abswordseg.cpp \
	convutil.cpp \
	filefilter.cpp \
	filterhtml.cpp \
	filterlatex.cpp \
	filterrtf.cpp \
	longwordseg.cpp \
	maxwordseg.cpp \
	wordseg.cpp

man_MANS = swath.1

swath_LDADD = \
	$(top_builddir)/conv/libconv.la \
	$(DATRIE_LIBS) \
	$(LTLIBOBJS)

