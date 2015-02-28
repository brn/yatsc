{
  'variables': {
    'visibility%': 'hidden',         # V8's visibility setting
    'target_arch%': 'ia32',          # set v8's target architecture
    'host_arch%': 'ia32',            # set v8's host architecture
    'library%': 'static_library',    # allow override to 'shared_library' for DLL/.so builds
    'component%': 'static_library',  # NB. these names match with what V8 expects
    'msvs_multi_core_compile': '1',  # we do enable multicore compiles, but not using the V8 way
  },

  'target_defaults': {
    'defines': ["PRODUCT_DIR=\"<(current_dir)\""],
    'default_configuration': 'Debug',
    'msbuild_toolset': 'v120',  # Visual Studio 2013 (v120)
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 3, # DLL debug
            'Optimization': 0, # /Od, no optimization
            'MinimalRebuild': 'true',
            'OmitFramePointers': 'false',
            'BasicRuntimeChecks': 3, # /RTC1
          },
          'VCLinkerTool': {
            'LinkIncremental': 2, # enable incremental linking
          },
        },
        'conditions' : [
          ['OS=="mac"', {
            'xcode_settings' : {
              'GCC_OPTIMIZATION_LEVEL' : '0'
            }
          }]
        ]
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'cflags': [ '-O3', '-fomit-frame-pointer', '-fdata-sections', '-ffunction-sections' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 0, # static release
            'EnableFiberSafeOptimizations': 'true',
            'Optimization': 3, # /Ox, full optimization
            'FavorSizeOrSpeed': 1, # /Ot, favour speed over size
            'InlineFunctionExpansion': 2, # /Ob2, inline anything eligible
            'WholeProgramOptimization': 'true', # /GL, whole program optimization, needed for LTCG
            'OmitFramePointers': 'true',
            'EnableFunctionLevelLinking': 'true',
            'EnableIntrinsicFunctions': 'true',
            'AdditionalOptions': [
              '/MP', # compile across multiple CPUs
            ],
          },
          'VCLibrarianTool': {
            'AdditionalOptions': [
              '/LTCG', # link time code generation
            ],
          },
          'VCLinkerTool': {
            'LinkTimeCodeGeneration': 1, # link-time code generation
            'OptimizeReferences': 2, # /OPT:REF
            'EnableCOMDATFolding': 2, # /OPT:ICF
            'LinkIncremental': 1, # disable incremental linking
          }
        },
        'conditions' : [
          ['OS=="mac"', {
            'xcode_settings' : {
              'GCC_OPTIMIZATION_LEVEL' : '3'
            },
          }]
        ]
      }
    },
    'msvs_settings': {
      'VCCLCompilerTool': {
        'StringPooling': 'true', # pool string literals
        'DebugInformationFormat': 3, # Generate a PDB
        'WarningLevel': 4,
        'BufferSecurityCheck': 'true',
        'ExceptionHandling': 1, # /EHsc
        'SuppressStartupBanner': 'true',
        'WarnAsError': 'false',
        'DisableSpecificWarnings': ['4127', '4714'], # /wd"4127" /wd"4714"
      },
      'VCLibrarianTool': {
        'AdditionalLibraryDirectories': ['./lib']
      },
      'VCLinkerTool': {
        'GenerateDebugInformation': 'true',
        'RandomizedBaseAddress': 2, # enable ASLR
        'DataExecutionPrevention': 2, # enable DEP
        'AllowIsolation': 'true',
        'SuppressStartupBanner': 'true',
        'target_conditions': [
          ['_type=="executable"', {
            'SubSystem': 1, # console executable
          }]
        ],
      },
    },
    'conditions': [
      ['"<(target_arch)"=="x64"', {
        'msvs_configuration_platform': 'x64'
      }],
      ['"<(target_arch)"=="x82"', {
        'msvs_configuration_platform': 'x82'
      }],
      ['OS == "win"', {
        'msvs_cygwin_shell': 0, # prevent actions from trying to use cygwin
        'defines': [
          'WIN32',
          # we don't really want VC++ warning us about
          # how dangerous C functions are...
          '_CRT_SECURE_NO_DEPRECATE',
          # ... or that C implementations shouldn't use
          # POSIX names
          '_CRT_NONSTDC_NO_DEPRECATE',
          'PLATFORM_WIN',
          'NOMINMAX'
        ]
      }],
      [ 'OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
        'defines': [
          'PLATFORM_POSIX',
          '__STDC_LIMIT_MACROS'
        ],
        'cflags': [ '-Wall' ],
        'cflags_cc': [ '-fno-rtti', '-fno-operator-names'],
        'conditions': [
          [ 'host_arch != target_arch and target_arch=="ia32"', {
            'cflags': [ '-m32' ],
            'ldflags': [ '-m32' ],
          }],
          [ 'OS=="linux"', {
            'cflags': [ '-ansi' ],
          }],
          [ 'OS=="solaris"', {
            'cflags': [ '-pthreads' ],
            'ldflags': [ '-pthreads' ],
          }, {
            'cflags': [ '-pthread' ],
            'ldflags': [ '-pthread' ],
          }]
        ],
      }],
      ['OS=="mac"', {
        'defines': [
          'PLATFORM_POSIX',
          'PLATFORM_MACH'
        ],
        'xcode_settings': {
          'target_conditions': [
            ['"<(target_arch)"=="x64"', {
              'ARCHS': ['x86_64'],
            }],
            ['"<(target_arch)"=="x82"', {
              'ARCHS': ['i386'],
            }],
          ],
          'ALWAYS_SEARCH_USER_PATHS': 'YES',
          'GCC_CW_ASM_SYNTAX': 'NO',                # No -fasm-blocks
          'GCC_DYNAMIC_NO_PIC': 'NO',               # No -mdynamic-no-pic
                                                    # (Equivalent to -fPIC)
          'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',        # -fno-exceptions
          'GCC_ENABLE_CPP_RTTI': 'NO',              # -fno-rtti
          'GCC_ENABLE_PASCAL_STRINGS': 'NO',        # No -mpascal-strings
          # GCC_INLINES_ARE_PRIVATE_EXTERN maps to -fvisibility-inlines-hidden
          'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES',
          'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',      # -fvisibility=hidden
          'GCC_THREADSAFE_STATICS': 'NO',           # -fno-threadsafe-statics
          'GCC_WARN_ABOUT_MISSING_NEWLINE': 'NO',  # -Wnewline-eof
          'PREBINDING': 'NO',                       # No -Wl,-prebind
          'USE_HEADERMAP': 'NO',
          'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0',
          'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
          'CLANG_CXX_LIBRARY': 'libc++', # libc++ requires OS X 10.7 or later
          'OTHER_CPLUSPLUSFLAGS': [
            '-fno-operator-names',
            '-std=c++11',
            '-stdlib=libc++',
            '-ferror-limit=100'
          ],
          'OTHER_LDFLAGS': ['-Wl,-search_paths_first'],
          'MACOSX_DEPLOYMENT_TARGET': '10.7',
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
            '-Wno-weak-vtables',
            '-Wno-extra-semi'
            "-Wno-pedantic"
          ]
        },
      }],
    ],
  },
}
