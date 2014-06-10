type=${1:-Debug}
sh build.osx.sh ${type} &&\
build/${type}/SourceStreamTest --gtest_color=yes&&\
build/${type}/UnicodeIteratorAdapterTest --gtest_color=yes &&\
build/${type}/ScannerTest --gtest_color=yes &&\
build/${type}/RegionsTest --gtest_color=yes
