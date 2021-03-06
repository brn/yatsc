type=${1:-Debug}
sh build.osx.sh ${type} &&\
build/${type}/IntrusiveRBTreeTest --gtest_color=yes &&\
build/${type}/AlignedHeapAllocatorTest --gtest_color=yes &&\
build/${type}/ChunkHeaderTest --gtest_color=yes&&\
build/${type}/ArenaTest --gtest_color=yes&&\
build/${type}/HeapAllocatorTest --gtest_color=yes&&\
build/${type}/SourceStreamTest --gtest_color=yes&&\
build/${type}/UnicodeIteratorAdapterTest --gtest_color=yes &&\
build/${type}/ScannerTest --gtest_color=yes &&\
build/${type}/NodeTest --gtest_color=yes &&\
build/${type}/ExpressionParseTest --gtest_color=yes &&\
build/${type}/DeclarationParseTest --gtest_color=yes &&\
build/${type}/StatementParseTest --gtest_color=yes
