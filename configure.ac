# Initialization

AC_INIT([m-stats], [0], [], [m-stats])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror])


# Libtool init and language selection

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_PROG_CXX

CXX_PROJECT_INIT


# Checks for dependencies

ROOTSYS_DEPS([glibs], [-lSpectrum -lMinuit])

CXX_PROJECT_DEPS([])


# Output

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
	Makefile
	m-stats.pc
	m-stats-config
	src/Makefile
	doc/Makefile doc/Doxyfile
])

AC_OUTPUT
