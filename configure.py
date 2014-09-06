#!/usr/bin/env python

import configbuilder

def BuildConfig() :
  builder = configbuilder.ConfigBuilder('src/config.h', always_build=True)
  builder.CheckHeader(True, ['thread'], 'thread required.')

  builder.CheckStruct(True, [
      {
        'name' : 'std::mutex',
        'header' : ['thread', 'mutex'],
        'struct': 'std::mutex'
      }
      ], 'mutex required.')

  builder.CheckStruct(True, [
      {
        'name' : 'std::condition_variable',
        'header' : ['thread', 'condition_variable'],
        'struct': 'std::condition_variable'
      }
      ], 'condition_variable required.')
  builder.CheckHeader(True, ['stdint.h'], '')
  builder.CheckHeader(True, ["unordered_map", "unordered_map"], 'unordered_map required.')
  builder.CheckStruct(True, [
      {
        'name' : 'std::bind',
        'header' : ['functional'],
        'function' : 'std::bind(fopen, std::placeholders::_1, "rb")'
        }], 'bind required.')
  builder.CheckHeader(True, ['type_traits'], 'type_traits required.')
  builder.CheckHeader(True, ['tuple', 'std/tuple'], 'tuple required.')
  builder.CheckStruct(True, [
      {
        'name' : 'std::function',
        'header' : ['functional'],
        'struct': 'std::function<int (int)>'
      }
    ], 'function required.')
  builder.CheckStruct(True, [
      {
        'name' : 'std::shared_ptr',
        'header' : ['memory'],
        'struct': 'std::shared_ptr<const char>'
      }
    ], 'shared_ptr required.')
  builder.CheckStruct(True, [
      {
        'name' : 'std::allocate_shared',
        'header' : ['memory'],
        'function': 'std::allocate_shared<char>(std::allocator<char>())'
      }
    ], 'allocate_shared required.')

  builder.CheckStruct(True, [
      {
        'name' : 'std::make_shared',
        'header' : ['memory'],
        'function': 'std::make_shared<char>(5)'
      }
    ], 'allocate_shared required.')

  builder.CheckStruct(True, [
      {
        'name' : 'std::unique_ptr',
        'header' : ['memory'],
        'struct': 'std::unique_ptr<int>'
      }
    ], 'unique_ptr is required.')

  builder.CheckStruct(False, [
    {
      'name': 'inline_attriute',
      'code' : '''
        inline __attribute__((always_inline)) int Test() {return 0;}
      '''
    },
    {
      'name': 'force_inline',
      'code' : '''
        inline __forceinline int Test() {return 0;}
      '''
    }
  ], 'inline attribute is required.')
  builder.CheckStruct(False, [
    {
      'name': 'unused_attribute',
      'code' : '''
        int Test(int x __attribute__((unused))) {return 0;}
      '''
    }
  ], 'unused attribute is required.')
  builder.CheckStruct(False, [
    {
      'name': 'HeapAlloc',
      'header' : ['Windows.h'],
      'function': 'HeapAlloc'
    },
    {
      'name': 'mmap',
      'header' : ['sys/mman.h'],
      'function': 'mmap'
    }
  ], 'HeapAlloc is required.')
  builder.CheckStruct(False, [
    {
      'name': 'VirtualAlloc',
      'header' : ['Windows.h'],
      'function': 'VirtualAlloc'
    }
  ], 'VirtualAlloc is required.')
  builder.CheckStruct(False, [
    {
      'name': 'munmap',
      'header' : ['sys/mman.h'],
      'function': 'munmap'
    }
  ], 'munmap is required.')
  builder.CheckStruct(False, [
    {
      'name': 'noexcept',
      'code' : '''
        int Test() noexcept {return 0;}
      '''
    }
  ], 'noexcept is required.')
  builder.CheckStruct(False, [
    {
      'name': 'vm_protect',
      'header' : ['mach/mach.h'],
      'function': 'vm_protect'
    }
  ], 'vm_protect is required.')
  builder.CheckStruct(False, [
    {
      'name': 'VM_MAKE_TAG',
      'header' : ['mach/vm_statistics.h'],
      'code': '''
        void test(){VM_MAKE_TAG(64);}
      '''
    }
  ], 'VM_MAKE_TAG is required.')
  builder.CheckStruct(True, [
    {
      'name': 'decltype',
      'code': '''
        static const int x = 0;
        decltype(x) v = 0;
      '''
    },
  ], 'decltype is required.')
  builder.CheckStruct(True, [
    {
      'name': 'std::alignment_of',
      'header': ['type_traits'],
      'code': '''
        static const size_t size = std::alignment_of<char>::value;
      '''
    },
    {
      'name': '__alignof',
      'code': '''
        static const size_t size = __alignof(char);
      '''
    },
  ], 'alignof is required.')
  builder.AddMacroCode("""
  #if defined(__x86_64__) || defined(_M_X64)
    #define PLATFORM_64BIT
  #elif defined(__i386) || defined(_M_IX86)
    #define PLATFORM_32BIT
  #endif
  """)
  builder.Build()

BuildConfig()
