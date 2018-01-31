#pragma once

namespace blz
{
  struct string
  {
    uint32_t size;
    uint32_t size2;
    union
    {
      char* ptr;
      char in_situ[sizeof (char const*)];
    };
    char const* data() const { return size2 & 0x80000000 ? in_situ : ptr; }
    char* data() { return size2 & 0x80000000 ? in_situ : ptr; } 
  };
  
  template<typename Ret, typename... Args>
    struct function
  {
    char unk[0x20-0x8];
    void* heap_copy;
    virtual ~function() = default;
    virtual Ret operator() (Args... args) = 0;
    virtual void dtor2() { return this->~function(); }
    virtual void* do_heap_alloc() = 0;
    virtual void clone (function<Ret, Args...>* other) = 0;
    virtual void* get_unk() { return &unk; }
  };
  
  template<typename Fun, typename Ret, typename... Args>
    struct static_function : function<Ret, Args...>
  {
    static_function() { heap_copy = this; }
    virtual Ret operator() (Args... args) override
    {
      return Fun{} (args...);
    }
    virtual void clone (function<Ret, Args...>* other) override
    {
      *other = *this;
    }
    virtual void* do_heap_alloc() { return new static_function<Fun, Ret, Args...>{}; }
  };
}