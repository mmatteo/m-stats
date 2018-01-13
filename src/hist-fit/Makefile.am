CLEANFILES = *.d *.so *_rdict.* *_rdict_* *.rootmap
MAINTAINERCLEANFILES = Makefile.in

# EXTRA_DIST = some extra files, e.g. test-data

lib_LTLIBRARIES = libm-stats-hist-fit.la

libm_stats_hist_fit_la_LIBADD = \
	 $(top_builddir)/src/core/libm-stats-core.la $(ROOTLIBS)

program_ldadd = libm-stats-hist-fit.la  $(libm_stats_hist_fit_la_LIBADD)

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/core 

libm_stats_hist_fit_la_SOURCES = \
        MSHistFit.cxx

libm_stats_hist_fit_la_headers = 

pkginclude_HEADERS = $(libm_stats_hist_fit_la_headers)

bin_PROGRAMS = m-stats-hist-fit

noinst_PROGRAMS =

m_stats_hist_fit_SOURCES = m-stats-hist-fit.cxx
m_stats_hist_fit_LDADD = $(program_ldadd)

rootmapdir = $(libdir)
rootmap_DATA =

libm_stats_hist_fit_la_SOURCES += libm-stats-hist-fit_rdict.cxx
rootmap_DATA += .libs/libm-stats-hist-fit.rootmap

rootmap_DATA += .libs/libm@ROOTPCMDASH@stats@ROOTPCMDASH@hist@ROOTPCMDASH@fit_rdict.pcm

libm-stats-hist-fit_rdict.cxx: $(libm_stats_hist_fit_la_headers) m-stats-hist-fit_LinkDef.h
	$(ROOTCLING) -f $@.tmp -s libm-stats-hist-fit@SHLIBEXT@ -rml libm-stats-hist-fit@SHLIBEXT@ -rmf libm-stats-hist-fit.rootmap.tmp -c $(CPPFLAGS) $(CXXFLAGS) -I$(includedir) $+
	@# Some magic to prefix header names with "$(PACKAGE)/", and only that, in dictionary and rootmap:
	$(GREP) -F -v '"'"`pwd`"'/",' $@.tmp | $(SED) 's|"\([^/"]*/\)*\([^/"]*[.]h\)",|"'$(PACKAGE)/'\2",| ; s|\\"\([^/"]*/\)*\([^/"]*[.]h\)\\"\\n"|\\"'$(PACKAGE)/'\2\\"\\n"|' > $@.tmp2
	$(SED) 's|\$$clingAutoload\$$\([^/"]*/\)*|$$clingAutoload$$'$(PACKAGE)'/|; /.*DICTPAYLOAD(.*/,/.*)DICTPAYLOAD.*/ s|#include "\([^/"]*/\)*\(.*\)"|#include <'$(PACKAGE)'/\2>|' $@.tmp2 > $@ && $(RM) $@.tmp $@.tmp2
	$(SED) 's|\(header  *\)\([^ ].*/\)*\([^ ].*[.]h\)|\1'$(PACKAGE)/'\3|' libm-stats-hist-fit.rootmap.tmp > libm-stats-hist-fit.rootmap && $(RM) libm-stats-hist-fit.rootmap.tmp

.libs/libm-stats-hist-fit.rootmap: libm-stats-hist-fit.la
	cp libm-stats-hist-fit.rootmap libm@ROOTPCMDASH@stats@ROOTPCMDASH@hist@ROOTPCMDASH@fit_rdict.pcm .libs/

.libs/libm@ROOTPCMDASH@stats@ROOTPCMDASH@hist@ROOTPCMDASH@fit_rdict.pcm: .libs/libm-stats-hist-fit.rootmap
