{
  'includes': ['commons.gypi'],
  'targets': [
    {
      'target_name': 'heap',
      'type': 'static_library',
      'product_name': 'heap',
      'include_dirs' : ['./lib', '/usr/local/include'],
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
      ],
    }
  ],
}
