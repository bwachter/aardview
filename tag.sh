#!/bin/sh
# tag.sh
# (c) 2018 Bernd Wachter <bwachter@lart.info>

OLD_VERSION=`git describe --dirty --tags 2>/dev/null`
if [ $? -ne 0 ]; then
    echo "Unable to read last tag"
    exit 1
fi

NEW_VERSION=`echo $OLD_VERSION | awk -f version.awk`

( cd aardview && ../write-version-header.sh $NEW_VERSION )

git add -f aardview/version.h
git commit -m "Bump to version $VERSION" aardview/version.h
git tag $NEW_VERSION
