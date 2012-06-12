MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
CLEANFILES = swathdic.tri

EXTRA_DIST = \
	update-dict.sh \
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
	$(TRIETOOL) swathdic add-list -e utf-8 $(srcdir)/swathdic.lst
