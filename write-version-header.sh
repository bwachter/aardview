#!/bin/sh
# write-version-header.sh
# (c) 2018 Bernd Wachter <bwachter@lart.info>

# in case of release-tagging this points to the commit just before
# bumping the version header
REF=`git rev-parse --short HEAD 2>/dev/null`
if [ $? -ne 0 ]; then
    echo "Apparently not a git repository, hoping a valid version header exists"
    exit 0
fi

VERSION=

# any argument is treated as version number to use
if [ -n "$1" ]; then
    VERSION=$1
else
    VERSION=`git describe --dirty --tags 2>/dev/null`
    if [ $? -ne 0 ]; then
        VERSION=$REF
        if [ $? -ne 0 ]; then
            VERSION="unknown"
        fi
    fi
fi

cat << EOF > version.h.new
#ifndef VERSION_H
#define VERSION_H
#define AARDVIEW_VERSION "$VERSION"
#define AARDVIEW_GIT_SHA "$REF"
#endif
EOF

diff -q version.h version.h.new >/dev/null 2>&1
if [ $? -ne 0 ]; then
    mv version.h.new version.h
    FILES=`grep -l 'version.h' *.cpp *.h`
    for FILE in $FILES; do
        touch $FILE
    done
else
    rm version.h.new
fi
