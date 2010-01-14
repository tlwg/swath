MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
CLEANFILES = swathdic.tri

EXTRA_DIST = \
	swathdic.abm \
	swathdic.lst

if ENABLE_DICT
pkgdata_DATA = \
	swathdic.tri
else
pkgdata_DATA =
endif

swathdic.tri: swathdic.lst
	if test ! -e swathdic.abm; then \
	  ln -s $(srcdir)/swathdic.abm . ; \
	fi
	rm -f swathdic.tri
	$(TRIETOOL) swathdic add-list -e tis-620 $(srcdir)/swathdic.lst
