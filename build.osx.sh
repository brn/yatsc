#!/usr/bin/sh

PROJECTS=(celero.xcodeproj heap.xcodeproj perf-test.xcodeproj test.xcodeproj)

for PROJ in ${PROJECTS[*]}; do
    if [ "${1}" = "clean" ] ; then
        xcodebuild -project $PROJ -configuration Release clean
        xcodebuild -project $PORJ -configuration Debug clean
    elif [ "${2}" = "64" ] ; then
        xcodebuild -project $PROJ -configuration ${1} arch=x86_64
    else
        xcodebuild -project $PROJ -configuration ${1}
    fi
    
    if [ $? != 0 ] ; then
        exit 1
    fi
done
