dnl -*- mode: autoconf -*- 
dnl
dnl Autoconf macro to resolve CERN ROOT System dependencies
dnl Synopsis:
dnl
dnl  ROOTSYS_DEPS([OPTIONS,[EXTRA-LIBS]])
dnl
dnl Requires root.m4

AC_DEFUN([ROOTSYS_DEPS],
[
	ROOTSYS_OPTIONS="$1"
	ROOTSYS_EXTRA_LIBS="$2"
	
	AC_REQUIRE([ROOT_PATH])
	AC_PATH_PROG(ROOTCLING, rootcling, false, $rootbin)
	if test "${ROOTCLING}" = false; then
		AC_MSG_ERROR([Need rootcling.]);
	fi
	
	ROOTSYS_CFLAGS="$ROOTCFLAGS $ROOTAUXCFLAGS"
	ROOTSYS_LIBS="-L$ROOTLIBDIR"
	if test "$ROOTSYS_OPTIONS" == "glibs" ; then
		ROOTSYS_LIBS="$ROOTSYS_LIBS $ROOTGLIBS"
	else
		ROOTSYS_LIBS="$ROOTSYS_LIBS $ROOTLIBS"
	fi
	ROOTSYS_LIBS="$ROOTSYS_LIBS $ROOTAUXLIBS $ROOTSYS_EXTRA_LIBS"
	
	DEP_CFLAGS="$DEP_CFLAGS $ROOTSYS_CFLAGS"
	DEP_LIBS="$ROOTSYS_LIBS $DEP_LIBS"

	ROOTPCMDASH="-"
	if test "${ROOTCLING}" != false ; then
		touch conftest_LinkDef.h
		"${ROOTCLING}" -f libconftest-foo_rdict.cxx -s libconftest-foo"${shrext_cmds}" conftest_LinkDef.h
		test -f libconftestmIfoo_rdict.pcm && ROOTPCMDASH="mI"
		rm -f conftest_LinkDef.h libconftest-foo_rdict.cxx libconftest-foo_rdict.pcm libconftestmIfoo_rdict.pcm
	fi
	AC_SUBST(ROOTPCMDASH)
])


#
# EOF
#
