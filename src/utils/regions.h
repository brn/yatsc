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
#include "../config.h"

namespace rasp {

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
 * This memory pool allocate object and array which public extends rasp::RegionalObject,
 * and all allocated object is dealloced by destructor, but if you want,
 * call Regions::Dealloc(void*) explicitly.
 *
 * @example
 * rasp::Regions p(1024);
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

  static const size_t kSizeBitSize = RASP_ALIGN_OFFSET(sizeof(SizeBit), kAlignment);
  static const uint8_t kDeallocedBit = 0x2;
  static const uint8_t kArrayBit = 0x1;
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
  explicit Regions(size_t size = 512);

  
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
  RASP_INLINE void Destroy() RASP_NOEXCEPT;
  

  /**
   * Free the specified pointer.
   * This function in fact not release memory.
   * This method add memory block to the free list and call destructor.
   * @param object The object pointer that must be allocated by Regions::Allocate[Array].
   */
  RASP_INLINE void Dealloc(void* object) RASP_NOEXCEPT;


  /**
   * Create new instance from Regions heap.
   * The instance which created by this method
   * must not use delete or free.
   */
  template <typename T, typename ... Args>
  RASP_INLINE T* New(Args ... args);


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
  RASP_INLINE double commited_bytes() RASP_NO_SE {
    return static_cast<double>(allocator_.commited_size());
  }


  /**
   * Get current allocated size by kilo byte.
   * @return The kilo byte expression.
   */
  RASP_INLINE double commited_kbytes() RASP_NO_SE {
    return static_cast<double>(allocator_.commited_size()) / 1024;
  }


  /**
   * Get current allocated size by mega byte.
   * @return The mega byte expression.
   */
  RASP_INLINE double commited_mbytes() RASP_NO_SE {
    return static_cast<double>(allocator_.commited_size()) / 1024 / 1024;
  }


  /**
   * Get current allocated heap size which include all unused space.
   * @return The byte expression.
   */
  RASP_INLINE double real_commited_bytes() RASP_NO_SE {
    return static_cast<double>(allocator_.real_commited_size());
  }


  /**
   * Get current allocated heap size which include all unused space.
   * @return The kilo byte expression.
   */
  RASP_INLINE double real_commited_kbytes() RASP_NO_SE {
    return static_cast<double>(allocator_.real_commited_size()) / 1024;
  }


  /**
   * Get current allocated heap size which include all unused space.
   * @return The mega byte expression.
   */
  RASP_INLINE double real_commited_mbytes() RASP_NO_SE {
    return static_cast<double>(allocator_.real_commited_size()) / 1024 / 1024;
  }
  
 private :


  /**
   * Return enough size memory block for specified size.
   * @return Unused memory block.
   */
  RASP_INLINE void* Allocate(size_t size);


  /**
   * Return enough size memory block for specified size.
   * @return Unused memory block.
   */
  RASP_INLINE void* AllocateArray(size_t size);


  /**
   * Advance pointer position
   */
  template <typename T>
  RASP_INLINE static void* PtrAdd(T* ptr, size_t size) {
    return reinterpret_cast<void*>(reinterpret_cast<Byte*>(ptr) + size);
  }


  /**
   * Destruct RegionalObject class instance by the proper method.
   */
  RASP_INLINE static void DestructRegionalObject(Regions::Header* header);


  /**
   * Allocate unused memory space from chunk.
   */
  inline Regions::Header* DistributeBlock(size_t size);
  
  
  /**
   * The memory block representation class.
   */
  class Chunk {
   public:
    typedef uint8_t VerificationTag;
    static const size_t kVerificationTagSize = RASP_ALIGN_OFFSET(sizeof(VerificationTag), kAlignment);
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
    inline static void Delete(Chunk* chunk) RASP_NOEXCEPT;
    

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
    RASP_INLINE bool HasEnoughSize(size_t needs) RASP_NO_SE;
  

    /**
     * Get memory block.
     * Must call HasEnoughSize before call this,
     * If the given size is over the block capacity,
     * this method cause the segfault.
     * @param needed size.
     * @returns aligned memory chunk.
     */
    Header* GetBlock(size_t reserve) RASP_NOEXCEPT;


    /**
     * Store last memory block.
     * @param block_begin The last allocated block.
     */
    RASP_INLINE void set_tail(Byte* block_begin) RASP_NOEXCEPT {
      tail_block_ = block_begin;
    }


    /**
     * Check wheter the given block is tail of the chunk or not.
     * @param block_begin New memory block.
     */
    RASP_INLINE bool IsTail(Byte* block_begin) RASP_NOEXCEPT {
      return tail_block_ == block_begin;
    }


    /**
     * Connect new chunk to the next link.
     * @param chunk New chunk.
     */
    RASP_INLINE void set_next(Chunk* chunk) RASP_NOEXCEPT {next_ = chunk;}


    /**
     * Return next link.
     */
    RASP_INLINE Chunk* next() RASP_NO_SE {return next_;}


    /**
     * Return current chunk size.
     */
    RASP_INLINE size_t size() RASP_NO_SE {return block_size_;}


    RASP_INLINE Byte* block() RASP_NOEXCEPT {return block_;}
    
  
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
    RASP_INLINE Regions::Chunk* head() RASP_NO_SE {return head_;}


    /**
     * Return tail of list.
     */
    RASP_INLINE Regions::Chunk* current() RASP_NO_SE {return current_;}


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
    inline void Unshift(Regions::Header* block) RASP_NOEXCEPT;


    /**
     * Swap head of free list to next and return last head.
     */
    inline Regions::Header* Shift() RASP_NOEXCEPT;


    RASP_INLINE bool HasHead() RASP_NO_SE {
      return free_head_ != nullptr;
    }


    RASP_INLINE Regions::FreeHeader* head() RASP_NOEXCEPT {
      return free_head_;
    }


    RASP_INLINE void Clear() RASP_NOEXCEPT {
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
    void Destroy() RASP_NOEXCEPT;


    /**
     * Deallocate specified ptr.
     * @param object The object which want to deallocate.
     */
    void Dealloc(void* object) RASP_NOEXCEPT;


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
    inline int FindBestFitBlockIndex(size_t size) RASP_NOEXCEPT;


    /**
     * Find out arena which was unlocked.
     */
    inline LocalArena* FindUnlockedArena() RASP_NOEXCEPT;


    /**
     * Allocate arena to tls or get unlocked arena.
     * @return Current thread local arena.
     */
    RASP_INLINE Regions::LocalArena* TlsAlloc();
    

    /**
     * Add an arena to linked list.
     * @param arena The arena which want to connect.
     */
    inline void StoreNewLocalArena(Regions::LocalArena* arena) RASP_NOEXCEPT;


    void IterateChunkList(Regions::ChunkList*) RASP_NOEXCEPT;


    inline static void TlsFree(void* arena) RASP_NOEXCEPT {
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
    RASP_INLINE LocalArena* next() RASP_NO_SE {
      return next_;
    }


    /**
     * Append Regions::LocalArena to next link.
     * @param An arena which want to connect.
     */
    RASP_INLINE void set_next(LocalArena* arena) RASP_NOEXCEPT {
      next_ = arena;
    }

    
    /**
     * Try lock arena.
     */
    RASP_INLINE bool AcquireLock() RASP_NOEXCEPT {
      return !lock_.test_and_set();
    }


    /**
     * Unlock arena.
     */
    RASP_INLINE void ReleaseLock() RASP_NOEXCEPT {
      lock_.clear();
    }


    /**
     * Get chunk.
     * @return Specific class chunk list.
     */
    RASP_INLINE ChunkList* chunk_list() RASP_NOEXCEPT {
      return &chunk_list_;
    }


    RASP_INLINE FreeChunkStack* free_chunk_stack(int index) RASP_NOEXCEPT {
      if (index <= kMaxSmallObjectsCount) {
        return &(free_chunk_stack_[index]);
      }
      return InitHugeFreeChunkStack(index);
    }


    RASP_INLINE FreeChunkStack* free_chunk_stack() RASP_NOEXCEPT {
      return free_chunk_stack_;
    }


    RASP_INLINE HugeChunkMap* huge_free_chunk_map() RASP_NOEXCEPT {
      return &huge_free_chunk_map_;
    }


    RASP_INLINE bool has_free_chunk(int index) RASP_NOEXCEPT {
      if (index <= kMaxSmallObjectsCount) {
        return free_chunk_stack_[index].HasHead();
      }
      return HasHugeFreeChunkStack(index);
    }


    /**
     * Find the most nearly size block from free list if size class is 0.
     * @param size Need
     */
    Regions::Header* FindFreeBlockFromHugeMap(int index) RASP_NOEXCEPT;


    Regions::FreeChunkStack* InitHugeFreeChunkStack(int index);


    RASP_INLINE bool HasHugeFreeChunkStack(int index) RASP_NOEXCEPT {
      return huge_free_chunk_map_.count(index) != 0;
    }


    RASP_INLINE Mmap* allocator() RASP_NOEXCEPT {
      return &mmap_;
    }


    /**
     * Add The Regions::LocalArena to free list of The Regions::CentralArena.
     */
    RASP_INLINE void Return();
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
} // namesapce rasp


#include "regions-inl.h"
#endif
