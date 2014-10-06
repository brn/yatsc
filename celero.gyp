{
  'variables': {
    'target_arch%': 'x86'
  },
  'targets': [
    {
      'target_name': 'celero',
      'type': 'static_library',
      'product_name': 'celero',
      'include_dirs' : ['./Celero/include'],
      'sources': [
        './Celero/src/Archive.cpp',
	'./Celero/src/Benchmark.cpp',
	'./Celero/src/Callbacks.cpp',
	'./Celero/src/Celero.cpp',
	'./Celero/src/Console.cpp',
	'./Celero/src/Distribution.cpp',
	'./Celero/src/Executor.cpp',
	'./Celero/src/JUnit.cpp',
	'./Celero/src/Print.cpp',
	'./Celero/src/Experiment.cpp',
	'./Celero/src/Result.cpp',
	'./Celero/src/ResultTable.cpp',
	'./Celero/src/Statistics.cpp',
	'./Celero/src/TestVector.cpp',
	'./Celero/src/TestFixture.cpp',
	'./Celero/src/Timer.cpp',
	'./Celero/src/Utilities.cpp'
      ],
    }
  ],
  'target_defaults': {
    'msbuild_toolset': 'v120',  # Visual Studio 2013 (v120)
    'defines': ['CELERO_STATIC'],
    'conditions': [
      ['"<(target_arch)"=="x64"', {
        'msvs_configuration_platform': 'x64'
      }],
      ['"<(target_arch)"=="x86"', {
        'msvs_configuration_platform': 'x86'
      }],
      ['OS == "win"', {
        'defines': ['_CRT_SECURE_NO_WARNINGS', '_VARIADIC_MAX=10']
      }],
      [ 'OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
        'cflags': [ '-Wall', '-std=c++11', '-std=gnu++11', '-pedantic']
      }],
    ],
    'xcode_settings': {
      'ARCHS': ['x86_64', 'i386'],
      'MACOSX_DEPLOYMENT_TARGET': '10.7',
      'OTHER_CPLUSPLUSFLAGS': [
        '-fno-operator-names',
        '-std=c++11',
        '-stdlib=libc++'
      ],
      'ALWAYS_SEARCH_USER_PATHS': 'YES',
      'GCC_CW_ASM_SYNTAX': 'NO',                # No -fasm-blocks
      'GCC_DYNAMIC_NO_PIC': 'NO',               # No -mdynamic-no-pic
                                                # (Equivalent to -fPIC)
      'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',        # -fno-exceptions
      'GCC_ENABLE_CPP_RTTI': 'YES',              # -fno-rtti
      'GCC_ENABLE_PASCAL_STRINGS': 'NO',        # No -mpascal-strings
      'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES',
      'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',      # -fvisibility=hidden
      'GCC_THREADSAFE_STATICS': 'NO',           # -fno-threadsafe-statics
      'GCC_WARN_ABOUT_MISSING_NEWLINE': 'NO',  # -Wnewline-eof
      'PREBINDING': 'NO',                       # No -Wl,-prebind
      'USE_HEADERMAP': 'NO',
      'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
      'CLANG_CXX_LIBRARY': 'libc++', # libc++ requires OS X 10.7 or later
      'WARNING_CFLAGS': [
        '-Weverything',
        '-Wall',
        '-Wendif-labels',
        '-W',
        '-Wno-unused-parameter',
        '-Wno-padded',
        '-Wno-switch-enum',
        '-Wno-undef',
        '-Wno-c++98-compat',
        '-Wno-format-nonliteral',
        '-Wno-weak-vtables'
      ]
    },
    'msvs_settings': {
      'VCCLCompilerTool': {
        'DisableSpecificWarnings': ['4251'], # /wd"4251"
        'StringPooling': 'true', # pool string literals
        'DebugInformationFormat': 3, # Generate a PDB
        'WarningLevel': 4,
        'BufferSecurityCheck': 'true',
        'ExceptionHandling': 1, # /EHsc
        'SuppressStartupBanner': 'true',
        'WarnAsError': 'false',
        'AdditionalOptions': [
          '/MP', # compile across multiple CPUs
        ],
      },
      'VCLibrarianTool': {
        'AdditionalLibraryDirectories': ['./lib']
      },
      'VCLinkerTool': {
        'RandomizedBaseAddress': 2, # enable ASLR
        'DataExecutionPrevention': 2, # enable DEP
        'AllowIsolation': 'true',
        'SuppressStartupBanner': 'true',
      },
    },
    'configurations': {
      'Debug': {
        'msvs_settins': {
          'VCLinkerTool': {
            'GenerateDebugInformation': 'true',
            'RuntimeLibrary': 0, # static debug
            'Optimization': 0, # /Od, no optimization
            'OmitFramePointers': 'false',
            'BasicRuntimeChecks': 3, # /RTC1
            'MinimalRebuild': 'true',
          },
        },
        'conditions' : [
          ['OS=="mac"', {
            'xcode_settings' : {
              'GCC_OPTIMIZATION_LEVEL' : '0'
            }
          }]
        ],
        'cflags': [ '-O0' '-g'],
      },
      'Release': {
        'msvs_settins': {
          'VCLinkerTool': {
            'GenerateDebugInformation': 'false',
            'RuntimeLibrary': 1, # static
            'EnableFiberSafeOptimizations': 'true',
            'Optimization': 3, # /Ox, full optimization
            'FavorSizeOrSpeed': 1, # /Ot, favour speed over size
            'InlineFunctionExpansion': 2, # /Ob2, inline anything eligible
            'WholeProgramOptimization': 'true', # /GL, whole program optimization, needed for LTCG
            'OmitFramePointers': 'true',
            'BasicRuntimeChecks': 3, # /RTC1
            'EnableFunctionLevelLinking': 'true',
            'EnableIntrinsicFunctions': 'true',
            'LinkTimeCodeGeneration': 1, # link-time code generation
            'OptimizeReferences': 2, # /OPT:REF
            'EnableCOMDATFolding': 2, # /OPT:ICF
            'LinkIncremental': 1, # disable incremental linking
            'MinimalRebuild': 'false',
          },
          'VCLibrarianTool': {
            'AdditionalOptions': [
              '/LTCG', # link time code generation
            ],
          },
        },
        'conditions' : [
          ['OS=="mac"', {
            'xcode_settings' : {
              'GCC_OPTIMIZATION_LEVEL' : '3'
            }
          }]
        ],
        'cflags': [ '-O3', '-fomit-frame-pointer', '-fdata-sections', '-ffunction-sections' ],
      }
    }
  }
}
