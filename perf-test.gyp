{
  'includes': ['commons.gypi'],
  'target_defaults': {
    'defines': ['CELERO_STATIC'],
    'ldflags': [ '-L./build/Debug' ],
    'configurations': {
      'Debug': {
        'msvs_settings': {
          'VCLinkerTool': {
            'AdditionalDependencies': [
              './Debug/lib/celero.lib',
            ]
          }
        },
        'xcode_settings': {
          'OTHER_LDFLAGS': ['./build/Debug/libcelero.a']
        }
      },
      'Release': {
        'msvs_settings': {
          'VCLinkerTool': {
            'AdditionalDependencies': [
              './Release/lib/celero.lib',
            ]
          }
        },
        'xcode_settings': {
          'OTHER_LDFLAGS': ['./build/Release/libcelero.a']
        }
      }
    }
  },
  'targets': [
    {
      'target_name': "heap_allocator_perf_test",
      'product_name': 'HeapAllocatorPerfTest',
      'type': 'executable',
      'defines' : ['UNIT_TEST=1'],
      'include_dirs': ['/usr/local/include', './lib', './Celero/include'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './perfs/memory/heap-allocator/heap-allocator-perf-test.cc',
        './src/utils/os.cc',
      ],
    },
    {
      'target_name': "intrusive_rbtree_perf_test",
      'product_name': 'IntrusiveRbtreePerfTest',
      'type': 'executable',
      'defines' : ['UNIT_TEST=1'],
      'include_dirs': ['/usr/local/include', './lib', './Celero/include'],
      'sources': [
        './src/utils/utils.cc',
        './src/utils/os.cc',
        './perfs/utils/intrusive-rb-tree-test.cc',
      ],
    }
  ]
}
