#!/bin/bash -e

# Sanity check:
if ! (test -e fill-in-template.sh) ; then
	echo "ERROR: Script must be run in package directory!" 1>&2
	exit 1
fi

echo -n "Tarname of package (e.g. \"some-project\"): "
read PACKAGE_TARNAME
if ! (echo "${PACKAGE_TARNAME}" | grep -q '^[[:alpha:]][-_[:alnum:]]*$') ; then
	echo "Invalid package tarname: \"${PACKAGE_TARNAME}\"" 1>&2
	exit 1
elif (echo "${PACKAGE_TARNAME}" | grep -q '[A-Z]') ; then
	echo "WARNING: Package tarname should be lower-case!" 1>&2
fi

echo -n "Full name of package (default: \"${PACKAGE_TARNAME}\"): "
read PACKAGE_NAME
PACKAGE_NAME=${PACKAGE_NAME:-${PACKAGE_TARNAME}}
if ! (echo "${PACKAGE_NAME}" | grep -q '^[[:alpha:]][-_ [:alnum:]]*$') ; then
	echo "Invalid package name: \"${PACKAGE_NAME}\"" 1>&2
	exit 1
fi

echo -n "C++ class namespace (default: \"tmpns\"): "
read CXX_NAMESPACE
CXX_NAMESPACE=${CXX_NAMESPACE:-tmpns}
if ! (echo "${CXX_NAMESPACE}" | grep -q '^[[:alpha:]][-_[:alnum:]]*$') ; then
	echo "Invalid C++ namespace name: \"${CXX_NAMESPACE}\"" 1>&2
	exit 1
fi

echo -n "Full name of author"
if [ "${NAME}" != "" ] ; then echo -n " (default: \"${NAME}\")"; fi
echo -n ": "
read AUTHOR_NAME
AUTHOR_NAME=${AUTHOR_NAME:-${NAME}}
if ! (echo "${AUTHOR_NAME}" | grep -q '^[[:alpha:]][ -_ [:alnum:]]*$') ; then
	echo "Invalid author name: \"${AUTHOR_NAME}\"" 1>&2
	exit 1
fi

echo -n "Email address of author"
if [ "${EMAIL}" != "" ] ; then echo -n " (default: \"${EMAIL}\")"; fi
echo -n ": "
read AUTHOR_EMAIL
AUTHOR_EMAIL=${AUTHOR_EMAIL:-${EMAIL}}
if ! (echo "${AUTHOR_EMAIL}" | grep -q '^[-_.@[:alnum:]]\+$') ; then
	echo "Invalid email address: \"${AUTHOR_EMAIL}\"" 1>&2
	exit 1
fi

echo -n "Short (one-line) description of package: "; read PACKAGE_DESC
if ! (echo "${PACKAGE_DESC}" | grep -q '^[^|]*$') ; then
	echo "Invalid characters in package description \"${PACKAGE_DESC}\"" 1>&2
	exit 1
fi

echo ""
echo ""
echo "Package: ${PACKAGE_TARNAME} (${PACKAGE_NAME})"
echo "Author: ${AUTHOR_NAME} <${AUTHOR_EMAIL}>"
echo "Description: ${PACKAGE_DESC}"

echo ""
echo -n "Proceed? (y/n): "
read GOOD_TO_GO
if ! (echo "${GOOD_TO_GO}" | grep -i -q '^y\|yes$') ; then
	echo "Abort!" 1>&2
	exit 1
fi



PACKAGE_VARNAME=`echo "${PACKAGE_TARNAME}" | sed 's/-/_/g'`
PACKAGE_RMAPNAME=`echo "${PACKAGE_TARNAME}" | sed 's/-/mI/g'`
DATE_YEAR=`date -u '+%Y'`


sed "s|Description: |Description: ${PACKAGE_DESC}|" -i ./template-project.pc.in
mv "template-project.pc.in" "${PACKAGE_TARNAME}.pc.in"
mv "template-project-config.in" "${PACKAGE_TARNAME}-config.in"

for f in \
	Makefile.am build-inst-run-test.sh configure.ac \
	src/Makefile.am src/template-project_LinkDef.h doc/Doxyfile.in
do
	sed "s|template-project|${PACKAGE_TARNAME}|g; s|template_project|${PACKAGE_VARNAME}|g; s|templatemIproject|${PACKAGE_RMAPNAME}|g" -i "$f"
	sed "s|Template Project|${PACKAGE_NAME}|" -i "$f"
done

mv "src/template-project_LinkDef.h" "src/${PACKAGE_TARNAME}_LinkDef.h"
mv "src/template-project.cxx" "src/${PACKAGE_TARNAME}.cxx"
mv "src/template_project.C" "src/${PACKAGE_VARNAME}.C"

for f in src/*.h src/*.cxx src/*.C; do
	sed "s|YEAR AUTHOR <EMAIL>|${DATE_YEAR} ${AUTHOR_NAME} <${AUTHOR_EMAIL}>|" -i "$f"
	sed "s|tmplns|${CXX_NAMESPACE}|" -i "$f"
done

echo -n "" > README

rm fill-in-template.sh

echo "Done." 1>&2

echo
echo "You can use build-inst-run-test.sh to test a full build, install" 1>&2
echo "run, uninstall, maintainer-clean cycle." 1>&2
echo
echo "Now it's time to remove the dummy class and executable and add your" 1>&2
echo "own code. Remove build-inst-run-test.sh, it only works for the dummy" 1>&2
echo "code." 1>&2
echo
