CLEANFILES = *.d *.so *_rdict.* *_rdict_* *.rootmap
MAINTAINERCLEANFILES = Makefile.in

# EXTRA_DIST = some extra files, e.g. test-data

lib_LTLIBRARIES = libm-stats.la

libm_stats_la_SOURCES = \
	MSConfig.cxx \
	MSDataPoint.cxx \
	MSMath.cxx \
	MSMinimizer.cxx \
	MSModel.cxx \
	MSModelPulls.cxx \
	MSModelTHnBMLF.cxx \
	MSPDFBuilderTHn.cxx \
	MSParameter.cxx

libm_stats_la_headers = \
	MSConfig.h \
	MSDataPoint.h \
	MSDataPointVector.h \
	MSMath.h \
	MSMinimizer.h \
	MSModel.h \
	MSModelPulls.h  \
	MSModelTHnBMLF.h \
	MSObject.h \
	MSPDFBuilderTHn.h \
	MSParameter.h 


pkginclude_HEADERS = $(libm_stats_la_headers)

bin_PROGRAMS =

noinst_PROGRAMS =

# m_stats_SOURCES =
# m_stats_LDADD =
# m_stats_LDFLAGS = -static


rootmapdir = $(libdir)
rootmap_DATA =


libm_stats_la_SOURCES += libm-stats_rdict.cxx
rootmap_DATA += .libs/libm-stats.rootmap

rootmap_DATA += .libs/libm@ROOTPCMDASH@stats_rdict.pcm

libm-stats_rdict.cxx: $(libm_stats_la_headers) m-stats_LinkDef.h
	$(ROOTCLING) -f $@.tmp -s libm-stats@SHLIBEXT@ -rml libm-stats@SHLIBEXT@ -rmf libm-stats.rootmap.tmp -c $(CPPFLAGS) $(CXXFLAGS) -I$(includedir) $+
	@# Some magic to prefix header names with "$(PACKAGE)/", and only that, in dictionary and rootmap:
	$(GREP) -F -v '"'"`pwd`"'/",' $@.tmp | $(SED) 's|"\([^/"]*/\)*\([^/"]*[.]h\)",|"'$(PACKAGE)/'\2",| ; s|\\"\([^/"]*/\)*\([^/"]*[.]h\)\\"\\n"|\\"'$(PACKAGE)/'\2\\"\\n"|' > $@.tmp2
	$(SED) 's|\$$clingAutoload\$$\([^/"]*/\)*|$$clingAutoload$$'$(PACKAGE)'/|; /.*DICTPAYLOAD(.*/,/.*)DICTPAYLOAD.*/ s|#include "\([^/"]*/\)*\(.*\)"|#include <'$(PACKAGE)'/\2>|' $@.tmp2 > $@ && $(RM) $@.tmp $@.tmp2
	$(SED) 's|\(header  *\)\([^ ].*/\)*\([^ ].*[.]h\)|\1'$(PACKAGE)/'\3|' libm-stats.rootmap.tmp > libm-stats.rootmap && $(RM) libm-stats.rootmap.tmp

.libs/libm-stats.rootmap: libm-stats.la
	cp libm-stats.rootmap libm@ROOTPCMDASH@stats_rdict.pcm .libs/

.libs/libm@ROOTPCMDASH@stats_rdict.pcm: .libs/libm-stats.rootmap
