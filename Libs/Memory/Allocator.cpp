#include "Allocator.h"
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <new>
#include <iostream>

namespace mem
{
  static const float s_round(0.49999f);

  CAllocator::CAllocator(size_t size) :
    memory_size(size), alloc_count(0)
  {
    internal_memory = new uchar[memory_size];
    memory_free = memory_size;

    allocated_size = memory_size - memory_free;
    block_count = memory_size / s_uMEM_ALIGNMENT;

    setup_headers();
  }
  // ------------------------------------
  void CAllocator::setup_headers()
  {
    assert(block_count >= 0);
    for (size_t index = 0; index < memory_size; index++)
    {
      auto* header_block_ptr = reinterpret_cast<mem::header_block*>(internal_memory + index);
      header_block_ptr->block_size = 0;
    }
  }
  // ------------------------------------
  void* CAllocator::alloc(size_t size)
  {
    assert(size != 0);
    if (!internal_memory)
    {
      return std::malloc(size);
    }

    size_t free_blocks_count = get_free_blocks();
    bool free_blocks = ((free_blocks_count * s_uMEM_ALIGNMENT) >= size);
    if (!free_blocks)
    {
      printf("Add extra memory!\n");
      throw std::bad_alloc();
    }

    // get address
    void* address = get_free_memory_block(size);

    // update header stats
    mem::header_block* header_block_ptr = get_free_header_block(size);
    if (size <= s_uBYTE_ALIGNMENT)
    {
      for (uint32_t index = 0; index < s_uBYTE_ALIGNMENT; index++)
      {
        if (header_block_ptr->get_size(index) == 0)
        {
          header_block_ptr->set_size(index, static_cast<uint32_t>(size));
          break;
        }
      }
    }
    else
    {
      header_block_ptr->block_size = static_cast<uint32_t>(size);
    }

    // update values
    allocated_size += size;
    memory_free -= size;
    alloc_count++;

    return address;
  }
  // ------------------------------------
  bool CAllocator::free(void* ptr, size_t size)
  {
    assert(ptr || size != 0);
    std::printf("Freeing: size = %zu\n", size);

    size_t it = 0;
    while (it != block_count)
    {

    }

    /* size_t free_block = -1;
     for (size_t index = 0; index < count; index++)
     {
       const mem::block_info& block = blocks[index];
       size_t block_size = block.end_offset - block.start_offset;
       bool equal_size = (block_size == size);
       void* current_ptr = (internal_memory + block.start_offset);

       if (equal_size && current_ptr == ptr)
       {
         free_block = index;
         break;
       }
     }*/

     // Not found!
     //assert(free_block != -1);

     // mark as free
     //mem::block_header& block = blocks[free_block];
     //block.free = true;

    allocated_size -= size;
    memory_free += size;
    alloc_count = (alloc_count - 1) <= 0 ? 0 : alloc_count--;

    return true;
  }
  // ------------------------------------
  const size_t CAllocator::get_free_blocks() const
  {
    float floating_offset = static_cast<float>(memory_free) / static_cast<float>(s_uMEM_ALIGNMENT);
    return static_cast<size_t>(std::round(floating_offset + s_round));
  }
  // ------------------------------------
  void CAllocator::print_memory()
  {
    assert(block_count >= 0);
    for (size_t index = 0; index < block_count; index++)
    {
      void* memory_block = get_memory_block(index);
      mem::header_block* header_block = get_header_block(index);
      uint32_t block_size = header_block->get_packed_size();
      printf("address: %p, block size: %u, Free: %d\n", memory_block, block_size, block_size < s_uBYTE_ALIGNMENT);
    }
  }
  // ------------------------------------
  void* CAllocator::get_memory_block(size_t index)
  {
    assert(block_count >= 0);
    size_t offset = sizeof(mem::header_block);
    void* memory_block = reinterpret_cast<uchar*>(internal_memory + offset);

    size_t it = 0;
    while (it != index)
    {
      // get header block
      mem::header_block* header_block_ptr = get_header_block(it);
      uint32_t current_size = header_block_ptr->get_packed_size();

      bool valid_size = current_size != INVALID_SIZE;
      bool out_of_bounds = valid_size ? current_size > s_uBYTE_ALIGNMENT : valid_size;
      uint32_t aligment = out_of_bounds ? current_size : s_uBYTE_ALIGNMENT;

      offset += static_cast<size_t>(aligment) + sizeof(header_block);
      assert(offset <= memory_size);
      memory_block = reinterpret_cast<uchar*>(internal_memory + offset);

      it++;
    }

    return memory_block;
  }
  // ------------------------------------
  void* CAllocator::get_free_memory_block(size_t size)
  {
    mem::header_block* free_header_block_ptr = get_free_header_block(size);
    assert(free_header_block_ptr);

    uchar* raw_ptr = reinterpret_cast<uchar*>(free_header_block_ptr);
    size_t offset = sizeof(mem::header_block) + free_header_block_ptr->get_packed_size();

    uchar* free_memory_block = reinterpret_cast<uchar*>(raw_ptr + offset);
    return free_memory_block;
  }
  // ------------------------------------
  mem::header_block* CAllocator::get_header_block(size_t index)
  {
    assert(block_count >= 0);
    mem::header_block* header_block_ptr = reinterpret_cast<mem::header_block*>(internal_memory);

    size_t it = 0, offset = 0;
    while (it != index)
    {
      uint32_t current_size = header_block_ptr->get_packed_size();
      bool valid_size = current_size != INVALID_SIZE;
      bool out_of_bounds = valid_size ? current_size > s_uBYTE_ALIGNMENT : valid_size;
      uint32_t aligment = out_of_bounds ? current_size : s_uBYTE_ALIGNMENT;

      offset += static_cast<size_t>(aligment) + sizeof(mem::header_block);
      assert(offset <= memory_size);
      header_block_ptr = reinterpret_cast<mem::header_block*>(internal_memory + offset);

      it++;
    }

    return header_block_ptr;
  }
  // ------------------------------------
  mem::header_block* CAllocator::get_free_header_block(size_t size)
  {
    assert(block_count >= 0);
    size_t it = 0;
    while (it != block_count)
    {
      mem::header_block* header_block_ptr = get_header_block(it);
      assert(header_block_ptr);

      uint32_t current_packed_size = header_block_ptr->get_packed_size();
      uint32_t target_size = static_cast<uint32_t>(current_packed_size + size);
      bool empty_size = current_packed_size == 0;
      bool free_block = (target_size <= s_uBYTE_ALIGNMENT) || (empty_size);
      if (free_block)
      {
        return header_block_ptr;
      }
      it++;
    }
    return nullptr;
  }
}
