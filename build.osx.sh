if [ "${1}" = "clean" ] ; then
    xcodebuild -project test.xcodeproj -configuration Release clean
    xcodebuild -project test.xcodeproj -configuration Debug clean
elif [ "${2}" = "64" ] ; then
    xcodebuild -project test.xcodeproj -configuration ${1} arch=x86_64
else
    xcodebuild -project test.xcodeproj -configuration ${1}
fi
    
