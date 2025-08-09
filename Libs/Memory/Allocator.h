#pragma once
#include <cstdint>

namespace mem
{
  /*
    cosas que hacer:
    1. que funcione todas alocaciones
    2. reajustar los bloques libres
    3. para hacer optimizacion cada cierto N alocaciones ordenary tener un indice para saber donde insertar memoria
  */

  struct header_block
  {
    uint32_t size;
    bool free;

    header_block() : size(0), free(true) {}
    ~header_block() {}
  } static invalid_block;

  static constexpr uint32_t s_uBYTE_ALIGNMENT = 8u;
  static constexpr uint32_t s_uMEM_ALIGNMENT = sizeof(header_block) + s_uBYTE_ALIGNMENT;

  class CAllocator
  {
  public:
    using uchar = unsigned char;

  public:
    explicit CAllocator(size_t size);
    ~CAllocator() { delete[] internal_memory; }

    void* alloc(size_t size);
    bool free(void* ptr, size_t size);

    const size_t get_allocated_size() const { return allocated_size; }
    const size_t get_memory_free() const { return memory_free; }
    const size_t get_memory_size() const { return memory_size; }

    void print_memory();

  private:
    void find_free_blocks(size_t size, size_t& begin_block, size_t& end_block);

    size_t FindAndReorder(size_t size);

    void setup_headers();
    void* get_memory_block(size_t index);
    header_block* get_header_block(size_t index);

  private:
    size_t allocated_size;
    size_t memory_free;

    size_t memory_size;
    size_t block_count;
    size_t alloc_count;

    uchar* internal_memory;
  };
}



