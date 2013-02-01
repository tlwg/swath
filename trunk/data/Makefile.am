MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
CLEANFILES = swathdic.lst swathdic.tri

DICT_SRC =  \
	tdict-city.txt \
	tdict-collection.txt \
	tdict-common.txt \
	tdict-country.txt \
	tdict-district.txt \
	tdict-geo.txt \
	tdict-history.txt \
	tdict-ict.txt \
	tdict-lang-ethnic.txt \
	tdict-proper.txt \
	tdict-science.txt \
	tdict-spell.txt \
	tdict-std-compound.txt \
	tdict-std.txt

EXTRA_DIST = \
	update-dict.sh \
	swathdic.abm \
	$(DICT_SRC)

if ENABLE_DICT
pkgdata_DATA = \
	swathdic.tri
else
pkgdata_DATA =
endif

swathdic.lst: ${DICT_SRC}
	cat $^ | LC_ALL=C sort -u > $@

swathdic.tri: swathdic.lst
	if test ! -e swathdic.abm; then \
	  ln -s $(srcdir)/swathdic.abm . ; \
	fi
	rm -f swathdic.tri
	$(TRIETOOL) swathdic add-list -e utf-8 swathdic.lst
