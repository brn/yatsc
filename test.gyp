{
  'includes': ['commons.gypi'],
  'conditions': [['OS!="mac"', {
  'make_global_settings': [
    ['CXX','/usr/bin/clang++'],
    ['LINK','/usr/bin/clang++'],
    ['CXXFLAGS', '-std=c++11 -stdlib=libc++']
  ]}]],
  'target_defaults': {
    'msvs_settings': {
      'VCCLCompilerTool': {
        'WarningLevel': '4', # /W4
      }
    },
    'xcode_settings': {
      'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
      'MACOSX_DEPLOYMENT_TARGET': '10.7', # OS X Deployment Target: 10.8
      'CLANG_CXX_LIBRARY': 'libc++', # libc++ requires OS X 10.7 or later
    },
  },
  'targets': [
    {
      'target_name': 'intrusive_rb_tree_test',
      'type': 'executable',
      'product_name': 'IntrusiveRBTreeTest',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/os.cc',
        './test/utils/intrusive-rb-tree-test.cc',
        './lib/gtest/gtest-all.cc',
        './test/test-main.cc',
      ],
      'xcode_settings': {
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'OTHER_CFLAGS': [
          "-std=c++11",
          "-stdlib=libc++"
        ],
      },
    },
    {
      'target_name': 'aligned_heap_allocator_test',
      'type': 'executable',
      'product_name': 'AlignedHeapAllocatorTest',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/tls.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './test/memory/aligned-heap-allocator-test.cc',
        './src/utils/utils.cc',
        './src/utils/os.cc',
        './lib/gtest/gtest-all.cc',
        './test/test-main.cc',
      ],
      'xcode_settings': {
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'OTHER_CFLAGS': [
          "-std=c++11",
          "-stdlib=libc++"
        ],
      },
    },
    {
      'target_name': 'chunk_header_test',
      'type': 'executable',
      'product_name': 'ChunkHeaderTest',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/tls.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './test/memory/heap-allocator/chunk-header-test.cc',
        './src/utils/utils.cc',
        './src/utils/os.cc',
        './lib/gtest/gtest-all.cc',
        './test/test-main.cc',
      ]
    },
    {
      'target_name': 'arena_test',
      'type': 'executable',
      'product_name': 'ArenaTest',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/tls.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './test/memory/heap-allocator/arena-test.cc',
        './src/utils/utils.cc',
        './src/utils/os.cc',
        './lib/gtest/gtest-all.cc',
        './test/test-main.cc',
      ]
    },
    {
      'target_name': 'heap_allocator_test',
      'type': 'executable',
      'product_name': 'HeapAllocatorTest',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/utils/os.cc',
        './test/memory/heap-allocator/heap-allocator-test.cc',
        './lib/gtest/gtest-all.cc',
        './test/test-main.cc',
      ],
    },
    {
      'target_name': 'path_test',
      'type': 'executable',
      'product_name': 'PathTest',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/utils/os.cc',
        './src/utils/path.cc',
        './test/utils/path-test.cc',
        './lib/gtest/gtest-all.cc',
        './test/test-main.cc',
      ],
    },
    {
      'target_name': 'scanner_test',
      'type': 'executable',
      'product_name': 'ScannerTest',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/utils/os.cc',
        './src/compiler-option.cc',
        './src/utils/environment.cc',
        './src/parser/token.cc',
        './src/parser/error-formatter.cc',
        './src/parser/sourcestream.cc',
        './lib/gtest/gtest-all.cc',
        './test/parser/scanner-keyword-scan-test.cc',
        './test/parser/scanner-operator-scan-test.cc',
        './test/parser/scanner-test.cc',
        './test/test-main.cc',
      ],
    },
    {
      'target_name': 'unicode_iterator_adapter_test',
      'product_name': 'UnicodeIteratorAdapterTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/utils/os.cc',
        './lib/gtest/gtest-all.cc',
        './test/parser/unicode-iterator-adapter-test.cc',
        './test/test-main.cc',
      ],
    },
    {
      'target_name': 'sourcestream_test',
      'product_name': 'SourceStreamTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/utils/os.cc',
        './src/parser/sourcestream.cc',
        './lib/gtest/gtest-all.cc',
        './test/parser/sourcestream-test.cc',
        './test/test-main.cc',
      ],
    },
    {
      'target_name': 'node_test',
      'product_name': 'NodeTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/utils/os.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './src/ir/scope.cc',
        './test/ir/node-test.cc',
        './test/test-main.cc'
      ],
    },
    {
      'target_name': 'type_expression_parser_test',
      'product_name': 'TypeExpressionParserTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/compiler/module-info.cc',
        './src/utils/path.cc',
        './src/utils/os.cc',
        './src/compiler-option.cc',
        './src/parser/token.cc',
        './src/parser/error-formatter.cc',
        './src/parser/sourcestream.cc',
        './src/compiler/type-registry.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './src/ir/scope.cc',
        './src/ir/types.cc',
        './test/parser/type-expression-parser-test.cc',
        './test/test-main.cc'
      ],
    },
    {
      'target_name': 'expression_parse_test',
      'product_name': 'ExpressionParseTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/compiler/module-info.cc',
        './src/utils/path.cc',
        './src/utils/os.cc',
        './src/compiler-option.cc',
        './src/parser/token.cc',
        './src/parser/error-formatter.cc',
        './src/parser/sourcestream.cc',
        './src/compiler/type-registry.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './src/ir/scope.cc',
        './src/ir/types.cc',
        './test/parser/expression-parse-test.cc',
        './test/test-main.cc'
      ],
    },
    {
      'target_name': 'declaration_parse_test',
      'product_name': 'DeclarationParseTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/compiler/module-info.cc',
        './src/utils/path.cc',
        './src/utils/os.cc',
        './src/compiler-option.cc',
        './src/parser/token.cc',
        './src/parser/error-formatter.cc',
        './src/parser/sourcestream.cc',
        './src/compiler/type-registry.cc',
        './src/utils/systeminfo.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './src/ir/scope.cc',
        './src/ir/types.cc',
        './test/parser/declaration-parse-test.cc',
        './test/test-main.cc'
      ],
    },
    {
      'target_name': 'statement_parse_test',
      'product_name': 'StatementParseTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/compiler/module-info.cc',
        './src/utils/path.cc',
        './src/utils/os.cc',
        './src/compiler-option.cc',
        './src/parser/token.cc',
        './src/parser/error-formatter.cc',
        './src/parser/sourcestream.cc',
        './src/compiler/type-registry.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './src/ir/scope.cc',
        './src/ir/types.cc',
        './test/parser/statement-parse-test.cc',
        './test/test-main.cc'
      ],
    },
    {
      'target_name': 'module_parse_test',
      'product_name': 'ModuleParseTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/compiler/module-info.cc',
        './src/utils/path.cc',
        './src/utils/os.cc',
        './src/compiler-option.cc',
        './src/parser/token.cc',
        './src/parser/error-formatter.cc',
        './src/parser/sourcestream.cc',
        './src/compiler/type-registry.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './src/ir/scope.cc',
        './src/ir/types.cc',
        './test/parser/module-parse-test.cc',
        './test/test-main.cc'
      ],
    },
    {
      'target_name': 'ambient_parser_test',
      'product_name': 'AmbientParserTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/compiler/module-info.cc',
        './src/utils/path.cc',
        './src/utils/os.cc',
        './src/compiler-option.cc',
        './src/parser/token.cc',
        './src/parser/error-formatter.cc',
        './src/parser/sourcestream.cc',
        './src/compiler/type-registry.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './src/ir/scope.cc',
        './src/ir/types.cc',
        './test/parser/ambient-parser-test.cc',
        './test/test-main.cc'
      ],
    },
    {
      'target_name': 'entire_parser_test',
      'product_name': 'EntireParserTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/compiler/module-info.cc',
        './src/utils/path.cc',
        './src/utils/os.cc',
        './src/compiler-option.cc',
        './src/parser/token.cc',
        './src/parser/error-formatter.cc',
        './src/parser/sourcestream.cc',
        './src/compiler/type-registry.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './src/ir/scope.cc',
        './src/ir/types.cc',
        './test/parser/parser-test.cc',
        './test/test-main.cc'
      ],
    },
    {
      'target_name': 'compiler_test',
      'product_name': 'CompilerTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './src/utils/os.cc',
        './src/utils/path.cc',
        './src/compiler-option.cc',
        './src/compiler/module-info.cc',
        './src/compiler/compiler.cc',
        './src/compiler/compilation-unit.cc',
        './src/compiler/thread-pool.cc',
        './src/compiler/channel.cc',
        './src/compiler/thread-pool-count.cc',
        './src/compiler/thread-pool-queue.cc',
        './src/compiler/type-registry.cc',
        './src/parser/sourcestream.cc',
        './src/parser/token.cc',
        './src/parser/error-formatter.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './src/ir/scope.cc',
        './src/ir/types.cc',
        './test/compiler/compiler-test.cc',
        './test/test-main.cc'
      ],
    },
  ] # targets
}
