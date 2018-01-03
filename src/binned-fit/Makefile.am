CLEANFILES = *.d *.so *_rdict.* *_rdict_* *.rootmap
MAINTAINERCLEANFILES = Makefile.in

# EXTRA_DIST = some extra files, e.g. test-data

lib_LTLIBRARIES = libm-stats-binned-fit.la

libm_stats_binned_fit_la_LIBADD = \
	 $(top_builddir)/src/core/libm-stats-core.la $(ROOTLIBS)

program_ldadd = libm-stats-binned-fit.la  $(libm_stats_binned_fit_la_LIBADD)

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/core 

libm_stats_binned_fit_la_SOURCES = \
        MSBinnedFit.cxx

libm_stats_binned_fit_la_headers = 

pkginclude_HEADERS = $(libm_stats_binned_fit_la_headers)

bin_PROGRAMS = m-stats-binned-fit

noinst_PROGRAMS =

m_stats_binned_fit_SOURCES = m-stats-binned-fit.cxx
m_stats_binned_fit_LDADD = $(program_ldadd)

rootmapdir = $(libdir)
rootmap_DATA =

libm_stats_binned_fit_la_SOURCES += libm-stats-binned-fit_rdict.cxx
rootmap_DATA += .libs/libm-stats-binned-fit.rootmap

rootmap_DATA += .libs/libm@ROOTPCMDASH@stats@ROOTPCMDASH@binned@ROOTPCMDASH@fit_rdict.pcm

libm-stats-binned-fit_rdict.cxx: $(libm_stats_binned_fit_la_headers) m-stats-binned-fit_LinkDef.h
	$(ROOTCLING) -f $@.tmp -s libm-stats-binned-fit@SHLIBEXT@ -rml libm-stats-binned-fit@SHLIBEXT@ -rmf libm-stats-binned-fit.rootmap.tmp -c $(CPPFLAGS) $(CXXFLAGS) -I$(includedir) $+
	@# Some magic to prefix header names with "$(PACKAGE)/", and only that, in dictionary and rootmap:
	$(GREP) -F -v '"'"`pwd`"'/",' $@.tmp | $(SED) 's|"\([^/"]*/\)*\([^/"]*[.]h\)",|"'$(PACKAGE)/'\2",| ; s|\\"\([^/"]*/\)*\([^/"]*[.]h\)\\"\\n"|\\"'$(PACKAGE)/'\2\\"\\n"|' > $@.tmp2
	$(SED) 's|\$$clingAutoload\$$\([^/"]*/\)*|$$clingAutoload$$'$(PACKAGE)'/|; /.*DICTPAYLOAD(.*/,/.*)DICTPAYLOAD.*/ s|#include "\([^/"]*/\)*\(.*\)"|#include <'$(PACKAGE)'/\2>|' $@.tmp2 > $@ && $(RM) $@.tmp $@.tmp2
	$(SED) 's|\(header  *\)\([^ ].*/\)*\([^ ].*[.]h\)|\1'$(PACKAGE)/'\3|' libm-stats-binned-fit.rootmap.tmp > libm-stats-binned-fit.rootmap && $(RM) libm-stats-binned-fit.rootmap.tmp

.libs/libm-stats-binned-fit.rootmap: libm-stats-binned-fit.la
	cp libm-stats-binned-fit.rootmap libm@ROOTPCMDASH@stats@ROOTPCMDASH@binned@ROOTPCMDASH@fit_rdict.pcm .libs/

.libs/libm@ROOTPCMDASH@stats@ROOTPCMDASH@binned@ROOTPCMDASH@fit_rdict.pcm: .libs/libm-stats-binned-fit.rootmap
