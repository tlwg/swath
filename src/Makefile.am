MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

INCLUDES = -I$(top_srcdir) $(DATRIE_CFLAGS)
DEFS = -DWORDSEGDATA_DIR=\"${pkgdatadir}\" -DVERSION=\"$(VERSION)\"

EXTRA_DIST = \
	abswordseg.h \
	dictpath.h \
	filefilter.h \
	filterhtml.h \
	filterlambda.h \
	filterlatex.h \
	filterrtf.h \
	filterx.h \
	longwordseg.h \
	maxwordseg.h \
	worddef.h \
	wordstack.h \
	swath.1

bin_PROGRAMS = swath

swath_SOURCES = \
	abswordseg.cpp \
	filefilter.cpp \
	filterhtml.cpp \
	filterlatex.cpp \
	filterrtf.cpp \
	filterlambda.cpp \
	filterx.cpp \
	longwordseg.cpp \
	maxwordseg.cpp \
	wordseg.cpp \
	wordstack.cpp

man_MANS = swath.1

# ** to use midatrie **
# swath_LDADD = @MIDATRIE_LIBS@
swath_LDADD = \
	$(top_builddir)/conv/libconv.la \
	$(DATRIE_LIBS)

