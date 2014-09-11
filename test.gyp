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
      'target_name': 'scanner_test',
      'type': 'executable',
      'product_name': 'ScannerTest',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/compiler-option.cc',
        './src/utils/systeminfo.cc',
        './src/utils/tls.cc',
        './src/utils/regions.cc',
        './src/utils/environment.cc',
        './src/utils/os.cc',
        './src/parser/token.cc',
        './lib/gtest/gtest-all.cc',
        './test/parser/scanner-keyword-scan-test.cc',
        './test/parser/scanner-operator-scan-test.cc',
        './test/parser/scanner-test.cc',
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
      'target_name': 'unicode_iterator_adapter_test',
      'product_name': 'UnicodeIteratorAdapterTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/os.cc',
        './src/utils/systeminfo.cc',
        './lib/gtest/gtest-all.cc',
        './test/parser/unicode-iterator-adapter-test.cc',
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
      'target_name': 'sourcestream_test',
      'product_name': 'SourceStreamTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/os.cc',
        './src/utils/systeminfo.cc',
        './src/parser/sourcestream.cc',
        './lib/gtest/gtest-all.cc',
        './test/parser/sourcestream-test.cc',
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
      'target_name': 'regions_test',
      'product_name': 'RegionsTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/systeminfo.cc',
        './src/utils/tls.cc',
        './src/utils/os.cc',
        './src/utils/regions.cc',
        './lib/gtest/gtest-all.cc',
        './test/utils/regions-test.cc',
        './test/test-main.cc'
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
      'target_name': 'node_test',
      'product_name': 'NodeTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/utils/systeminfo.cc',
        './src/utils/tls.cc',
        './src/utils/os.cc',
        './src/utils/regions.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './test/ir/node-test.cc',
        './test/test-main.cc'
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
      'target_name': 'expression_parse_test',
      'product_name': 'ExpressionParseTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/compiler-option.cc',
        './src/parser/token.cc',
        './src/utils/systeminfo.cc',
        './src/utils/tls.cc',
        './src/utils/os.cc',
        './src/utils/regions.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './test/parser/expression-parse-test.cc',
        './test/test-main.cc'
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
      'target_name': 'declaration_parse_test',
      'product_name': 'DeclarationParseTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/compiler-option.cc',
        './src/parser/token.cc',
        './src/utils/systeminfo.cc',
        './src/utils/tls.cc',
        './src/utils/os.cc',
        './src/utils/regions.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './test/parser/declaration-parse-test.cc',
        './test/test-main.cc'
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
      'target_name': 'statement_parse_test',
      'product_name': 'StatementParseTest',
      'type': 'executable',
      'include_dirs' : ['./lib', '/usr/local/include'],
      'defines' : ['GTEST_HAS_RTTI=0', 'UNIT_TEST=1'],
      'sources': [
        './src/compiler-option.cc',
        './src/parser/token.cc',
        './src/utils/systeminfo.cc',
        './src/utils/tls.cc',
        './src/utils/os.cc',
        './src/utils/regions.cc',
        './src/utils/environment.cc',
        './lib/gtest/gtest-all.cc',
        './src/ir/node.cc',
        './test/parser/statement-parse-test.cc',
        './test/test-main.cc'
      ],
      'xcode_settings': {
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'OTHER_CFLAGS': [
          "-std=c++11",
          "-stdlib=libc++"
        ],
      },
    },
  ] # targets
}
