AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

MAINTAINERCLEANFILES = Makefile.in \
	aclocal.m4 ar-lib config.guess config.h.in config.h.in~ config.sub \
	configure depcomp install-sh ltmain.sh missing compile \
	m4/libtool.m4 m4/lt~obsolete.m4 m4/ltoptions.m4 m4/ltsugar.m4 \
	m4/ltversion.m4

EXTRA_DIST = autogen.sh

SUBDIRS = \
	src/core \
	src/hist-fit \
	doc

dist_doc_DATA = README COPYING AUTHORS ChangeLog INSTALL NEWS
dist_bin_SCRIPTS = m-stats-config

if COND_PKGCONFIG
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = m-stats.pc
endif
