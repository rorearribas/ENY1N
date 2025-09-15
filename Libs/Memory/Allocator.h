#pragma once
#include <cstdint>
#include <cstdlib>

namespace mem
{
  /*
    cosas que hacer:
    1. que funcione todas alocaciones
    2. reajustar los bloques libres
    3. para hacer optimizacion cada cierto N alocaciones ordenary tener un indice para saber donde insertar memoria
  */

#define INVALID_SIZE 0xCDCDCDCD
  static constexpr uint32_t s_uBYTE_ALIGNMENT = 8u;

  struct header_block
  {
    size_t block_size;

    void set_size(int idx, uint32_t value)
    {
      const size_t mask = 0xF;
      const size_t shift = idx * 4;

      block_size &= ~(mask << shift);
      block_size |= ((value & mask) << shift);
    }
    uint32_t get_size(int idx) const
    {
      const size_t mask = 0xF;
      const size_t shift = idx * 4;
      return (block_size >> shift) & mask;
    }
    uint32_t get_packed_size() const
    {
      uint32_t packed_size = 0, index = 0;
      while (index != s_uBYTE_ALIGNMENT) { packed_size += get_size(index++); }
      return packed_size;
    }

    header_block() : block_size(INVALID_SIZE) {}
    ~header_block() {}
  };

  static constexpr uint32_t s_uMEM_ALIGNMENT = sizeof(header_block) + s_uBYTE_ALIGNMENT;

  class CAllocator
  {
  public:
    using uchar = unsigned char;

  public:
    explicit CAllocator(size_t size);
    ~CAllocator() {}

    void* alloc(size_t size);
    bool free(void* ptr, size_t size);

    const size_t get_allocated_size() const { return allocated_size; }
    const size_t get_memory_size() const { return memory_size; }
    const size_t get_memory_free() const { return memory_free; }

    const size_t get_free_blocks() const;
    void print_memory();

  private:
    void setup_headers();
    void* get_memory_block(size_t index);
    void* get_free_memory_block(size_t size);

    header_block* get_header_block(size_t index);
    header_block* get_free_header_block(size_t size);

  private:
    size_t allocated_size;
    size_t memory_free;

    size_t memory_size;
    size_t block_count;
    size_t alloc_count;

    uchar* internal_memory;
  };
}



