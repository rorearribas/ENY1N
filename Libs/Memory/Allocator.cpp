#include "Allocator.h"

#include <cstdio>
#include <cassert>
#include <new>
#include <iostream>

namespace mem
{
#define INVALID_SIZE 0xCDCDCDCD;
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
    for (size_t index = 0; index < block_count; index++)
    {
      mem::header_block* header = get_header_block(index);
      header->size = 0;
      header->free = true;
    }
  }
  // ------------------------------------
  void* CAllocator::alloc(size_t size)
  {
    size_t test = sizeof(mem::header_block);

    assert(size != 0);
    if (!internal_memory)
    {
      return std::malloc(size);
    }

    float offset = static_cast<float>(memory_free) / static_cast<float>(s_uMEM_ALIGNMENT);
    size_t free_blocks_count = static_cast<size_t>(std::round(offset + s_round));
    bool blocks_free = (free_blocks_count * s_uMEM_ALIGNMENT) >= size;
    if (!blocks_free)
    {
      throw std::bad_alloc{};
    }

    // get header
    mem::header_block* _header_block = get_header_block(alloc_count);
    _header_block->size = size;
    _header_block->free = false;

    // get address
    void* address = get_memory_block(alloc_count);

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

    size_t free_block = 0;
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

    return true;
  }

  void CAllocator::find_free_blocks(size_t size, size_t& begin_block, size_t& end_block)
  {
    //bool finding_blocks = false;
    //for (size_t index = 0; index < alloc_count; index++)
    //{
    //  mem::block_header* _block_header = get_block_header(index);
    //  bool enough_size = (s_uBYTE_ALIGNMENT >= size);

    //  if (_block_header->free && enough_size)
    //  {
    //    size_t current_size = _block_header->size;
    //    size_t new_size = current_size + size;

    //    bool free_block = new_size != block.end_offset;

    //    if (!free_block)
    //    {
    //      block = mem::invalid_block;
    //      alloc_count = index;
    //    }
    //    else
    //    {
    //      block.start_offset = new_offset; // re-order
    //    }
    //  }
    //}
  }

  /*mem::block_header* CAllocator::FindFreeBlock(size_t size)
  {

  }*/

  //  //  if (finding_blocks)
  //  //  {
  //  //    bytes_offset += s_iMEM_ALIGNMENT;
  //  //  }
  //  //}
  //}

  size_t CAllocator::FindAndReorder(size_t size)
  {
    //for (size_t block_idx = 0; block_idx < count; block_idx++)
    //{
    //  mem::block_info& block = blocks[block_idx];
    //  size_t block_size = block.end_offset - block.start_offset;
    //  bool enough_size = (block_size >= size);

    //  if (block.free && enough_size)
    //  {
    //    size_t start_offset = block.start_offset;
    //    size_t new_offset = start_offset + size;
    //    bool free_block = new_offset != block.end_offset;

    //    if (!free_block)
    //    {
    //      block = mem::invalid_block;
    //      count = block_idx;
    //    }
    //    else
    //    {
    //      block.start_offset = new_offset; // re-order
    //    }

    //    return start_offset;
    //  }
    //}
    //return allocated_size;

    return 0;
  }
  // ------------------------------------
  void CAllocator::print_memory()
  {
    assert(block_count >= 0);
    for (size_t index = 0; index < block_count; index++)
    {
      void* block = get_memory_block(index);
      mem::header_block* header = get_header_block(index);
      printf("address: %p, block size: %u, Free: %d\n", block, header->size, header->free);
    }
  }
  // ------------------------------------
  void* CAllocator::get_memory_block(size_t index)
  {
    assert(block_count >= 0);
    size_t offset = sizeof(mem::header_block);
    void* ptr = internal_memory + offset;

    size_t it = 0;
    while (it != index)
    {
      // get header block
      mem::header_block* _header_block = get_header_block(it);

      uint32_t current_size = _header_block->size;
      bool valid_size = current_size != INVALID_SIZE;
      bool out_of_bounds = valid_size ? current_size > s_uBYTE_ALIGNMENT : valid_size;
      uint32_t aligment = out_of_bounds ? current_size : s_uBYTE_ALIGNMENT;

      offset += static_cast<size_t>(aligment) + sizeof(header_block);
      assert(offset <= memory_size);
      ptr = reinterpret_cast<mem::header_block*>(internal_memory + offset);

      it++;
    }

    return ptr;
  }
  // ------------------------------------
  mem::header_block* CAllocator::get_header_block(size_t index)
  {
    assert(block_count >= 0);
    mem::header_block* _header_block = reinterpret_cast<mem::header_block*>(internal_memory);

    size_t it = 0, offset = 0;
    while (it != index)
    {
      uint32_t current_size = _header_block->size;
      bool valid_size = current_size != INVALID_SIZE;
      bool out_of_bounds = valid_size ? current_size > s_uBYTE_ALIGNMENT : valid_size;
      uint32_t aligment = out_of_bounds ? current_size : s_uBYTE_ALIGNMENT;

      offset += static_cast<size_t>(aligment) + sizeof(mem::header_block);
      assert(offset <= memory_size);
      _header_block = reinterpret_cast<mem::header_block*>(internal_memory + offset);

      it++;
    }

    return _header_block;
  }
}