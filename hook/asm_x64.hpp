#pragma once

#include "patching.hpp"

inline namespace asm_x64
{
  namespace
  {
    template<size_t n, typename T>
      constexpr T nth_byte(T const& x)
    {
      return (x >> (CHAR_BIT * n)) & 0xFF;
    }
  }
  
  using nop = patch<0x90>;
  struct movups_xmm0_heap
  {
    size_t _offset;
    patch<0x0F, 0x10, 0x05> _opcode;
    var<uint32_t> _target;
    dynpatch<decltype (_target)> _target_patch;
    movups_xmm0_heap (size_t offset, size_t target_offset)
      : _offset (offset)
      , _opcode (offset)
      , _target (offset + 3)
      , _target_patch (_target, (target_offset - (offset + 7u)))
    {}
  };
  struct movups_xmm1_heap
  {
    size_t _offset;
    patch<0x0F, 0x10, 0x0D> _opcode;
    var<uint32_t> _target;
    dynpatch<decltype (_target)> _target_patch;
    movups_xmm1_heap (size_t offset, size_t target_offset)
      : _offset (offset)
      , _opcode (offset)
      , _target (offset + 3)
      , _target_patch (_target, (target_offset - (offset + 7u)))
    {}
  };
  template<intptr_t stack> using movups_rbp_xmm0 = patch<0x0F, 0x11, 0x45, nth_byte<0> (-stack)>;
  template<intptr_t stack> using movups_rbp_xmm1 = patch<0x0F, 0x11, 0x4D, nth_byte<0> (-stack)>;
  template<intptr_t stack> using movups_rbp_xmm14 = patch<0x44, 0x0F, 0x11, 0x75, nth_byte<0> (-stack)>;
  template<unsigned char stack> using movups_rsp_xmm14 = patch<0x44, 0x0F, 0x11, 0x74, 0x24, stack>;
  using movups_xmm14_xmm0 = patch<0x44, 0x0f, 0x10, 0xf0>;
}