{
  'includes': ['commons.gypi'],
  'target_defaults': {
    'defines': ['CELERO_STATIC'],
    'configurations': {
      'Debug': {
        'msvs_settings': {
          'VCLinkerTool': {
            'AdditionalDependencies': [
              './Debug/lib/celero.lib',
              'Psapi.Lib'
            ]
          }
        }
      },
      'Release': {
        'msvs_settings': {
          'VCLinkerTool': {
            'AdditionalDependencies': [
              './Release/lib/celero.lib',
              'Psapi.Lib'
            ]
          }
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
      'include_dirs': ['./lib', './Celero/include'],
      'sources': [
        './src/utils/tls.cc',
        './src/utils/systeminfo.cc',
        './src/memory/virtual-heap-allocator.cc',
        './src/memory/aligned-heap-allocator.cc',
        './src/memory/heap-allocator/chunk-header.cc',
        './src/memory/heap-allocator/arena.cc',
        './src/memory/heap-allocator/heap-allocator.cc',
        './perfs/memory/heap-allocator/heap-allocator-perf-test.cc',
        './src/utils/os.cc',
        './lib/gtest/gtest-all.cc',
      ],
    }
  ]
}
