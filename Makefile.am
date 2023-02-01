ACLOCAL_AMFLAGS = -I m4

SUBDIRS = lib conv src data tests

EXTRA_DIST = \
	build-aux/git-version-gen	\
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in

dist-hook:
	echo $(VERSION) > $(distdir)/VERSION

