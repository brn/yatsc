/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2013 Taketoshi Aono(brn)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef UTILS_POOL_H_
#define UTILS_POOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <unordered_map>
#include <atomic>
#include <new>
#include "utils.h"
#include "tls.h"
#include "mmap.h"
#include "systeminfo.h"
#include "../config.h"

namespace yatsc {

class Regions;


/**
 * Base class of the Regions allocatable object.
 */
class RegionalObject {
 public:
  /**
   * virtual destructor.
   */
  virtual ~RegionalObject() {}
};


/**
 * Fast thread safe memory allocator.
 * All memory allocateded from heap(mmap/VirtualAlloc).
 * This memory pool allocate object and array which public extends yatsc::RegionalObject,
 * and all allocated object is dealloced by destructor, but if you want,
 * call Regions::Dealloc(void*) explicitly.
 *
 * @example
 * yatsc::Regions p(1024);
 * RegionalObjectExtendClass* poolable = new(&p) RegionalObjectExtendClass();
 * // something special.
 * p.Destroy();
 * // If do not call Regions::Destroy(void),
 * // that called in Regions destructor.
 */
class Regions : private Uncopyable {
 private:
  class Chunk;
  class FreeHeader;
  class Header;
  class ChunkList;
  class FreeChunkStack;
  class CentralArena;
  typedef Mmap::MmapStandardAllocator<std::pair<const size_t, Regions::FreeChunkStack*>> HugeChunkAllocator;
  typedef std::unordered_map<size_t, Regions::FreeChunkStack*,
                             std::hash<size_t>,
                             std::equal_to<size_t>,
                             HugeChunkAllocator> HugeChunkMap;

#ifdef PLATFORM_64BIT
  typedef uint64_t SizeBit;
  typedef uint64_t Size;
  static const uint64_t kTagRemoveBit = ~uint64_t(3);
  static const uint64_t kMaxAllocatableSize = UINT64_MAX;
  static const uint64_t kDeallocedMask = ~uint64_t(2);
#elif defined(PLATFORM_32BIT)
  typedef uint32_t SizeBit;
  typedef uint32_t Size;
  static const uint64_t kTagRemoveBit = ~uint32_t(3);
  static const uint32_t kMaxAllocatableSize = UINT32_MAX;
  static const uint64_t kDeallocedMask = ~uint32_t(2);
#endif

  static const size_t kSizeBitSize = YATSC_ALIGN_OFFSET(sizeof(SizeBit), kAlignment);
  static const uint8_t kDeallocedBit = 0x2;
  static const uint8_t kArrayBit = 0x1;
  static const uint8_t kPtrBit = 0x3;
  static const uint32_t kInvalidPointer = 0xDEADC0DE;
  static const int kMaxSmallObjectsCount = 30;
  static const int kValueOffset;
  static const size_t kFreeHeaderSize;
  static const size_t kHeaderSize;
  
 public :
  /**
   * Constructor
   * @param size The hint of each chunk size.
   */
  explicit Regions(size_t size = SystemInfo::GetPageSize());

  
  ~Regions() {Destroy();}


  Regions(Regions&& regions)
      : allocator_(std::move(regions.allocator_)),
        central_arena_(regions.central_arena_),
        size_(regions.size_) {
    ASSERT(true, size_ <= kMaxAllocatableSize);
    deleted_.clear();
    regions.deleted_.test_and_set();
    regions.central_arena_ = nullptr;
  }
  

  Regions& operator = (Regions&& regions);


  /**
   * Free all allocated memory.
   */
  YATSC_INLINE void Destroy() YATSC_NOEXCEPT;
  

  /**
   * Free the specified pointer.
   * This function in fact not release memory.
   * This method add memory block to the free list and call destructor.
   * @param object The object pointer that must be allocated by Regions::Allocate[Array].
   */
  YATSC_INLINE void Dealloc(void* object) YATSC_NOEXCEPT;


  /**
   * Create new instance from Regions heap.
   * The instance which created by this method
   * must not use delete or free.
   */
  template <typename T, typename ... Args>
  YATSC_INLINE T* New(Args ... args);


  /**
   * Create bulk memory block from Regions heap.
   * The instance which created by this method
   * must not use delete or free.
   */
  template <typename T>
  YATSC_INLINE T* NewPtr(size_t num);


  /**
   * Create new array of instance from Regions heap.
   * The instance which created by this method
   * must not use delete or free.
   */
  template <typename T, typename ... Args>
  inline T* NewArray(size_t size, Args ... args);
  

  /**
   * Get current allocated size by byte.
   * @return The byte expression.
   */
  YATSC_INLINE double commited_bytes() YATSC_NO_SE {
    return static_cast<double>(allocator_.commited_size());
  }


  /**
   * Get current allocated size by kilo byte.
   * @return The kilo byte expression.
   */
  YATSC_INLINE double commited_kbytes() YATSC_NO_SE {
    return static_cast<double>(allocator_.commited_size()) / 1024;
  }


  /**
   * Get current allocated size by mega byte.
   * @return The mega byte expression.
   */
  YATSC_INLINE double commited_mbytes() YATSC_NO_SE {
    return static_cast<double>(allocator_.commited_size()) / 1024 / 1024;
  }


  /**
   * Get current allocated heap size which include all unused space.
   * @return The byte expression.
   */
  YATSC_INLINE double real_commited_bytes() YATSC_NO_SE {
    return static_cast<double>(allocator_.real_commited_size());
  }


  /**
   * Get current allocated heap size which include all unused space.
   * @return The kilo byte expression.
   */
  YATSC_INLINE double real_commited_kbytes() YATSC_NO_SE {
    return static_cast<double>(allocator_.real_commited_size()) / 1024;
  }


  /**
   * Get current allocated heap size which include all unused space.
   * @return The mega byte expression.
   */
  YATSC_INLINE double real_commited_mbytes() YATSC_NO_SE {
    return static_cast<double>(allocator_.real_commited_size()) / 1024 / 1024;
  }
  
 private :


  /**
   * Return enough size memory block for specified size.
   * @return Unused memory block.
   */
  YATSC_INLINE void* Allocate(size_t size);


  /**
   * Return enough size memory block for specified size.
   * @return Unused memory block.
   */
  YATSC_INLINE void* AllocatePtr(size_t size);


  /**
   * Return enough size memory block for specified size.
   * @return Unused memory block.
   */
  YATSC_INLINE void* AllocateArray(size_t size);


  /**
   * Advance pointer position
   */
  template <typename T>
  YATSC_INLINE static void* PtrAdd(T* ptr, size_t size) {
    return reinterpret_cast<void*>(reinterpret_cast<Byte*>(ptr) + size);
  }


  /**
   * Destruct RegionalObject class instance by the proper method.
   */
  YATSC_INLINE static void DestructRegionalObject(Regions::Header* header);


  /**
   * Allocate unused memory space from chunk.
   */
  inline Regions::Header* DistributeBlock(size_t size, bool ptr = false);
  
  
  /**
   * The memory block representation class.
   */
  class Chunk {
   public:
    typedef uint8_t VerificationTag;
    static const size_t kVerificationTagSize = YATSC_ALIGN_OFFSET(sizeof(VerificationTag), kAlignment);
    static const uint8_t kVerificationBit = 0xAA;

    /**
     * Instantiate Chunk from heap.
     * @param size The block size.
     */
    inline static Chunk* New(size_t size, Mmap* allocator);


    /**
     * Delete Chunk.
     * @param chunk delete target.
     */
    inline static void Delete(Chunk* chunk) YATSC_NOEXCEPT;
    

    /**
     * Constructor
     * @param block Unused block allocated by Mmap.
     * @param size block size.
     */
    Chunk(Byte* block, size_t size)
        : block_size_(size),
          used_(0u),
          block_(block),
          tail_block_(nullptr),
          next_(nullptr) {}

  
    ~Chunk() = default;


    /**
     * Remove all chunks.
     */
    void Destruct();
  

    /**
     * Check the chunk has the enough size to allocate given size.
     * @param needs needed size.
     * @returns whether the chunk has the enough memory block or not.
     */
    YATSC_INLINE bool HasEnoughSize(size_t needs) YATSC_NO_SE;
  

    /**
     * Get memory block.
     * Must call HasEnoughSize before call this,
     * If the given size is over the block capacity,
     * this method cause the segfault.
     * @param needed size.
     * @returns aligned memory chunk.
     */
    Header* GetBlock(size_t reserve) YATSC_NOEXCEPT;


    /**
     * Store last memory block.
     * @param block_begin The last allocated block.
     */
    YATSC_INLINE void set_tail(Byte* block_begin) YATSC_NOEXCEPT {
      tail_block_ = block_begin;
    }


    /**
     * Check wheter the given block is tail of the chunk or not.
     * @param block_begin New memory block.
     */
    YATSC_INLINE bool IsTail(Byte* block_begin) YATSC_NOEXCEPT {
      return tail_block_ == block_begin;
    }


    /**
     * Connect new chunk to the next link.
     * @param chunk New chunk.
     */
    YATSC_INLINE void set_next(Chunk* chunk) YATSC_NOEXCEPT {next_ = chunk;}


    /**
     * Return next link.
     */
    YATSC_INLINE Chunk* next() YATSC_NO_SE {return next_;}


    /**
     * Return current chunk size.
     */
    YATSC_INLINE size_t size() YATSC_NO_SE {return block_size_;}


    YATSC_INLINE Byte* block() YATSC_NOEXCEPT {return block_;}
    
  
   private :

    size_t block_size_;
    size_t used_;
    Byte* block_;
    Byte* tail_block_;
    Chunk* next_;
  };


  /**
   * The linked list of chunk.
   */
  class ChunkList {
   public:
    ChunkList()
        : head_(nullptr),
          current_(nullptr) {}


    /**
     * Return head of list.
     */
    YATSC_INLINE Regions::Chunk* head() YATSC_NO_SE {return head_;}


    /**
     * Return tail of list.
     */
    YATSC_INLINE Regions::Chunk* current() YATSC_NO_SE {return current_;}


    /**
     * Create new chunk and connect
     * if current chunk not has enough size to allocate given size.
     * @param size Need size
     * @param default_size default size of the chunk if size class is zero
     * @param mmap allocator
     */
    inline Regions::Header* AllocChunkIfNecessary(size_t size, Mmap* mmap, Regions::CentralArena* arena);
      
   private:
    Regions::Chunk* head_;
    Regions::Chunk* current_;
  };


  class LocalArena;


  class FreeChunkStack: private Uncopyable {
   public:
    FreeChunkStack()
        : free_head_(nullptr){}
    ~FreeChunkStack() = default;
    
    /**
     * Connect memory block to tail of free list and replace tail.
     * @param block Dealloced memory block.
     */
    inline void Unshift(Regions::Header* block) YATSC_NOEXCEPT;


    /**
     * Swap head of free list to next and return last head.
     */
    inline Regions::Header* Shift() YATSC_NOEXCEPT;


    YATSC_INLINE bool HasHead() YATSC_NO_SE {
      return free_head_ != nullptr;
    }


    YATSC_INLINE Regions::FreeHeader* head() YATSC_NOEXCEPT {
      return free_head_;
    }


    YATSC_INLINE void Clear() YATSC_NOEXCEPT {
      free_head_ = nullptr;
    }
    
    
   private:
    Regions::FreeHeader* free_head_;
    SpinLock tree_lock_;
  };
  

  /**
   * The arena which is allocated in global space.
   */
  class CentralArena {
   public:
    /**
     * Constructor
     * @param mmap allocator
     */
    CentralArena(Mmap* mmap)
        : arena_head_(nullptr),
          arena_tail_(nullptr),
          mmap_(mmap) {
      tls_ = tls_once_init_(&TlsFree);
      huge_chunk_allocator_ = huge_chunk_allocator_once_init_(mmap_);
      huge_free_chunk_map_ = huge_free_chunk_map_once_init_(*huge_chunk_allocator_);
    }


    ~CentralArena() = default;


    /**
     * Get an arena from tls or allocate new one.
     * @param size Need size.
     * @param default_size Default chunk size.
     */
    inline Regions::Header* Commit(size_t size, size_t default_size);


    /**
     * Remove all arena.
     */
    void Destroy() YATSC_NOEXCEPT;


    /**
     * Deallocate specified ptr.
     * @param object The object which want to deallocate.
     */
    void Dealloc(void* object) YATSC_NOEXCEPT;


    /**
     * Move all LocalArena free chunks to CentralArena free chunks,
     * and unlock the LocalArena.
     * @param arena The arena belong to thread which is exited.
     */
    inline void CollectGarbage(Regions::LocalArena* arena);


    /**
     * Move all LocalArena::HugeFreeChunkMap to CentralArena::HugeFreeChunkMap.
     * @param arena The arena belong to thread which is exited.
     */
    inline void FreeMap(Regions::LocalArena* arena);


    /**
     * Find central free chunk which best fit to the given size.
     * @param size The size which want to allocate.
     */
    inline Regions::Header* FindFreeChunk(size_t size);
    
   private:


    /**
     * Return index of chunk list which fit to given size.
     * @param size The size which want to allocate.
     */
    inline int FindBestFitBlockIndex(size_t size) YATSC_NOEXCEPT;


    /**
     * Find out arena which was unlocked.
     */
    inline LocalArena* FindUnlockedArena() YATSC_NOEXCEPT;


    /**
     * Allocate arena to tls or get unlocked arena.
     * @return Current thread local arena.
     */
    YATSC_INLINE Regions::LocalArena* TlsAlloc();
    

    /**
     * Add an arena to linked list.
     * @param arena The arena which want to connect.
     */
    inline void StoreNewLocalArena(Regions::LocalArena* arena) YATSC_NOEXCEPT;


    void IterateChunkList(Regions::ChunkList*) YATSC_NOEXCEPT;


    inline static void TlsFree(void* arena) YATSC_NOEXCEPT {
      reinterpret_cast<Regions::LocalArena*>(arena)->Return();
    }
    

    LocalArena* arena_head_;
    LocalArena* arena_tail_;

    Mmap* mmap_;
    Regions::FreeChunkStack central_free_chunk_stack_[kMaxSmallObjectsCount];
    
    HugeChunkAllocator* huge_chunk_allocator_;
    LazyInitializer<HugeChunkAllocator> huge_chunk_allocator_once_init_;
    
    HugeChunkMap* huge_free_chunk_map_;
    LazyInitializer<HugeChunkMap> huge_free_chunk_map_once_init_;
    
    ThreadLocalStorage::Slot* tls_;
    LazyInitializer<ThreadLocalStorage::Slot> tls_once_init_;
    
    SpinLock central_free_arena_lock_;
    SpinLock central_huge_free_map_lock_;
    SpinLock dealloc_lock_;
  };
  

  /**
   * The thread local arena.
   */
  class LocalArena {
   public:
    /**
     * Constructor
     * @param central_arena The central arena.
     * @param mmap Allocator
     */
    inline LocalArena(CentralArena* central_arena, HugeChunkAllocator* huge_chunk_allocator);
    inline ~LocalArena();


    /**
     * Return next Regions::LocalArena.
     */
    YATSC_INLINE LocalArena* next() YATSC_NO_SE {
      return next_;
    }


    /**
     * Append Regions::LocalArena to next link.
     * @param An arena which want to connect.
     */
    YATSC_INLINE void set_next(LocalArena* arena) YATSC_NOEXCEPT {
      next_ = arena;
    }

    
    /**
     * Try lock arena.
     */
    YATSC_INLINE bool AcquireLock() YATSC_NOEXCEPT {
      return !lock_.test_and_set();
    }


    /**
     * Unlock arena.
     */
    YATSC_INLINE void ReleaseLock() YATSC_NOEXCEPT {
      lock_.clear();
    }


    /**
     * Get chunk.
     * @return Specific class chunk list.
     */
    YATSC_INLINE ChunkList* chunk_list() YATSC_NOEXCEPT {
      return &chunk_list_;
    }


    YATSC_INLINE FreeChunkStack* free_chunk_stack(int index) YATSC_NOEXCEPT {
      if (index <= kMaxSmallObjectsCount) {
        return &(free_chunk_stack_[index]);
      }
      return InitHugeFreeChunkStack(index);
    }


    YATSC_INLINE FreeChunkStack* free_chunk_stack() YATSC_NOEXCEPT {
      return free_chunk_stack_;
    }


    YATSC_INLINE HugeChunkMap* huge_free_chunk_map() YATSC_NOEXCEPT {
      return &huge_free_chunk_map_;
    }


    YATSC_INLINE bool has_free_chunk(int index) YATSC_NOEXCEPT {
      if (index <= kMaxSmallObjectsCount) {
        return free_chunk_stack_[index].HasHead();
      }
      return HasHugeFreeChunkStack(index);
    }


    /**
     * Find the most nearly size block from free list if size class is 0.
     * @param size Need
     */
    Regions::Header* FindFreeBlockFromHugeMap(int index) YATSC_NOEXCEPT;


    Regions::FreeChunkStack* InitHugeFreeChunkStack(int index);


    YATSC_INLINE bool HasHugeFreeChunkStack(int index) YATSC_NOEXCEPT {
      return huge_free_chunk_map_.count(index) != 0;
    }


    YATSC_INLINE Mmap* allocator() YATSC_NOEXCEPT {
      return &mmap_;
    }


    /**
     * Add The Regions::LocalArena to free list of The Regions::CentralArena.
     */
    YATSC_INLINE void Return();
   private:
    Mmap mmap_;
    CentralArena* central_arena_;
    std::atomic_flag lock_;
    ChunkList chunk_list_;
    FreeChunkStack free_chunk_stack_[kMaxSmallObjectsCount];
    HugeChunkMap huge_free_chunk_map_;
    LocalArena* next_;
  };
  
  Mmap allocator_;
  
  // The chunk list.
  CentralArena* central_arena_;
  LazyInitializer<CentralArena> central_arena_once_init_;
  

  size_t size_;
  std::atomic_flag deleted_;
  SpinLock tree_lock_;
};

} // namesapce yatsc

#include "regions-inl.h"
#endif
