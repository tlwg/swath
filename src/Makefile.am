MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

# ** to use midatrie **
# INCLUDES = @MIDATRIE_CFLAGS@
INCLUDES = -I$(top_srcdir)
DEFS = -DWORDSEGDATA_DIR=\"@datadir@\"

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
	dictpath.cpp \
	filefilter.cpp \
	filterhtml.cpp \
	filterlatex.cpp \
	filterrtf.cpp \
	filterlambda.cpp \
	filterx.cpp \
	longwordseg.cpp \
	maxwordseg.cpp \
	worddef.cpp \
	wordseg.cpp \
	wordstack.cpp

man_MANS = swath.1

# ** to use midatrie **
# swath_LDADD = @MIDATRIE_LIBS@
swath_LDADD = \
	$(top_builddir)/conv/libconv.la \
	$(top_builddir)/trie/libtrie.la \
	$(top_builddir)/vmem/libvmem.la \
	$(top_builddir)/misc/libmisc.la

