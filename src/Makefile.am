CLEANFILES = *.d *.so *_rdict.* *_rdict_* *.rootmap
MAINTAINERCLEANFILES = Makefile.in

# EXTRA_DIST = some extra files, e.g. test-data

lib_LTLIBRARIES = libtemplate-project.la

libtemplate_project_la_SOURCES = \
	HelloWorld.cxx

libtemplate_project_la_headers = \
	HelloWorld.h

pkginclude_HEADERS = $(libtemplate_project_la_headers)

bin_PROGRAMS = template-project

noinst_PROGRAMS =

template_project_SOURCES = template-project.cxx
template_project_LDADD = libtemplate-project.la
# template_project_LDFLAGS = -static


rootmapdir = $(libdir)
rootmap_DATA =


libtemplate_project_la_SOURCES += libtemplate-project_rdict.cxx
rootmap_DATA += .libs/libtemplate-project.rootmap

if WITH_CLING

rootmap_DATA += .libs/libtemplatemIproject_rdict.pcm

libtemplate-project_rdict.cxx: $(libtemplate_project_la_headers) template-project_LinkDef.h
	$(ROOTCLING) -f $@.tmp -s libtemplate-project@SHLIBEXT@ -rml libtemplate-project@SHLIBEXT@ -rmf libtemplate-project.rootmap -c $(CPPFLAGS) $(CXXFLAGS) -I$(includedir) $+
	@# Some magic to prefix header names with "$(PACKAGE)/", and only that, in dictionary and rootmap:
	$(GREP) -F -v '"'"`pwd`"'/",' $@.tmp | $(SED) 's|"\([^"]*/\)\?\([^/"]*[.]h\)",|"'$(PACKAGE)/'\2",| ; s|\\"\([^"]*/\)\?\([^/"]*[.]h\)\\"\\n"|\\"'$(PACKAGE)/'\2\\"\\n"|' > $@ && $(RM) $@.tmp
	$(SED) 's|\$$clingAutoload\$$\([^/""]\+/\)*|$$clingAutoload$$'$(PACKAGE)'/|; /.*DICTPAYLOAD(.*/,/.*)DICTPAYLOAD.*/ s|#include "\([^/"]\+/\)*\(.*\)"|#include <'$(PACKAGE)'/\2>|' -i $@
	$(SED) 's|\(header \+\)\([^ ].*/\)\?\([^ ].*[.]h\)|\1'$(PACKAGE)/'\3|' -i libtemplate-project.rootmap

.libs/libtemplate-project.rootmap: libtemplate-project.la
	cp libtemplate-project.rootmap libtemplatemIproject_rdict.pcm .libs/

.libs/libtemplatemIproject_rdict.pcm: .libs/libtemplate-project.rootmap

else # not WITH_CLING

libtemplate-project_rdict.cxx: $(libtemplate_project_la_headers) template-project_LinkDef.h
	$(ROOTCINT) -f $@ -c -p $(CPPFLAGS) $(CXXFLAGS) $+

.libs/libtemplate-project.rootmap: libtemplate-project.la | $(libtemplate_project_la_headers) template-project_LinkDef.h
	$(RLIBMAP) -o $@ -l .libs/libtemplate-project@SHLIBEXT@ -c $|

endif # WITH_CLING
