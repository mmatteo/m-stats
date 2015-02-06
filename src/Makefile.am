CLEANFILES = *.d *.so *_rdict.* *_rdict_* *.rootmap
MAINTAINERCLEANFILES = Makefile.in

# EXTRA_DIST = some extra files, e.g. test-data

lib_LTLIBRARIES = libm-stats.la

libm_stats_la_SOURCES =

libm_stats_la_headers =

pkginclude_HEADERS = $(libm_stats_la_headers)

bin_PROGRAMS =

noinst_PROGRAMS =

# m_stats_SOURCES = m-stats.cxx
# m_stats_LDADD = libm-stats.la
# m_stats_LDFLAGS = -static


rootmapdir = $(libdir)
rootmap_DATA =


libm_stats_la_SOURCES += libm-stats_rdict.cxx
rootmap_DATA += .libs/libm-stats.rootmap

if WITH_CLING

rootmap_DATA += .libs/libmmIstats_rdict.pcm

libm-stats_rdict.cxx: $(libm_stats_la_headers) m-stats_LinkDef.h
	$(ROOTCLING) -f $@.tmp -s libm-stats@SHLIBEXT@ -rml libm-stats@SHLIBEXT@ -rmf libm-stats.rootmap -c $(CPPFLAGS) $(CXXFLAGS) -I$(includedir) $+
	@# Some magic to prefix header names with "$(PACKAGE)/", and only that, in dictionary and rootmap:
	$(GREP) -F -v '"'"`pwd`"'/",' $@.tmp | $(SED) 's|"\([^"]*/\)\?\([^/"]*[.]h\)",|"'$(PACKAGE)/'\2",| ; s|\\"\([^"]*/\)\?\([^/"]*[.]h\)\\"\\n"|\\"'$(PACKAGE)/'\2\\"\\n"|' > $@ && $(RM) $@.tmp
	$(SED) 's|\$$clingAutoload\$$\([^/""]\+/\)*|$$clingAutoload$$'$(PACKAGE)'/|; /.*DICTPAYLOAD(.*/,/.*)DICTPAYLOAD.*/ s|#include "\([^/"]\+/\)*\(.*\)"|#include <'$(PACKAGE)'/\2>|' -i $@
	$(SED) 's|\(header \+\)\([^ ].*/\)\?\([^ ].*[.]h\)|\1'$(PACKAGE)/'\3|' -i libm-stats.rootmap

.libs/libm-stats.rootmap: libm-stats.la
	cp libm-stats.rootmap libmmIstats_rdict.pcm .libs/

.libs/libmmIstats_rdict.pcm: .libs/libm-stats.rootmap

else # not WITH_CLING

libm-stats_rdict.cxx: $(libm_stats_la_headers) m-stats_LinkDef.h
	$(ROOTCINT) -f $@ -c -p $(CPPFLAGS) $(CXXFLAGS) $+

.libs/libm-stats.rootmap: libm-stats.la | $(libm_stats_la_headers) m-stats_LinkDef.h
	$(RLIBMAP) -o $@ -l .libs/libm-stats@SHLIBEXT@ -c $|

endif # WITH_CLING
