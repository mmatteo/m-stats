CLEANFILES = *.d *.so *_rdict.* *_rdict_* *.rootmap
MAINTAINERCLEANFILES = Makefile.in

# EXTRA_DIST = some extra files, e.g. test-data

lib_LTLIBRARIES = libm-stats-binnedfit.la

libm_stats_binnedfit_la_LIBADD = \
	 $(top_builddir)/src/core/libm-stats-core.la $(ROOTLIBS)

program_ldadd = libm-stats-binnedfit.la  $(libm_stats_binnedfit_la_LIBADD)

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/core 

libm_stats_binnedfit_la_SOURCES = \
        MSBinnedFit.cxx

libm_stats_binnedfit_la_headers = 

pkginclude_HEADERS = $(libm_stats_binnedfit_la_headers)

bin_PROGRAMS = m-stats-binnedfit

noinst_PROGRAMS =

m_stats_binnedfit_SOURCES = m-stats-binnedfit.cxx
m_stats_binnedfit_LDADD = $(program_ldadd)

rootmapdir = $(libdir)
rootmap_DATA =

libm_stats_binnedfit_la_SOURCES += libm-stats-binnedfit_rdict.cxx
rootmap_DATA += .libs/libm-stats-binnedfit.rootmap

rootmap_DATA += .libs/libm@ROOTPCMDASH@stats@ROOTPCMDASH@binnedfit_rdict.pcm

libm-stats-binnedfit_rdict.cxx: $(libm_stats_binnedfit_la_headers) m-stats-binnedfit_LinkDef.h
	$(ROOTCLING) -f $@.tmp -s libm-stats-binnedfit@SHLIBEXT@ -rml libm-stats-binnedfit@SHLIBEXT@ -rmf libm-stats-binnedfit.rootmap.tmp -c $(CPPFLAGS) $(CXXFLAGS) -I$(includedir) $+
	@# Some magic to prefix header names with "$(PACKAGE)/", and only that, in dictionary and rootmap:
	$(GREP) -F -v '"'"`pwd`"'/",' $@.tmp | $(SED) 's|"\([^/"]*/\)*\([^/"]*[.]h\)",|"'$(PACKAGE)/'\2",| ; s|\\"\([^/"]*/\)*\([^/"]*[.]h\)\\"\\n"|\\"'$(PACKAGE)/'\2\\"\\n"|' > $@.tmp2
	$(SED) 's|\$$clingAutoload\$$\([^/"]*/\)*|$$clingAutoload$$'$(PACKAGE)'/|; /.*DICTPAYLOAD(.*/,/.*)DICTPAYLOAD.*/ s|#include "\([^/"]*/\)*\(.*\)"|#include <'$(PACKAGE)'/\2>|' $@.tmp2 > $@ && $(RM) $@.tmp $@.tmp2
	$(SED) 's|\(header  *\)\([^ ].*/\)*\([^ ].*[.]h\)|\1'$(PACKAGE)/'\3|' libm-stats-binnedfit.rootmap.tmp > libm-stats-binnedfit.rootmap && $(RM) libm-stats-binnedfit.rootmap.tmp

.libs/libm-stats-binnedfit.rootmap: libm-stats-binnedfit.la
	cp libm-stats-binnedfit.rootmap libm@ROOTPCMDASH@stats@ROOTPCMDASH@binnedfit_rdict.pcm .libs/

.libs/libm@ROOTPCMDASH@stats@ROOTPCMDASH@binnedfit_rdict.pcm: .libs/libm-stats-binnedfit.rootmap
