
namespace 
{
  template<bool, typename> struct maybe_unprotect;
  template<typename T> struct maybe_unprotect<true, T>
  {
    static void apply(void* p, DWORD* old)
    {
      VirtualProtect(p, sizeof (T), PAGE_EXECUTE_READWRITE, old);
    }
    static void remove(void* p, DWORD old)
    {
      DWORD ignored;
      VirtualProtect(p, sizeof (T), old, &ignored);
    }
  };
  template<typename T> struct maybe_unprotect<false, T>
  {
    static void apply(void* p, DWORD* old)
    {
    }
    static void remove(void* p, DWORD old)
    {
    }
  };
}

namespace 
{
  char* module_base()
  {
    MODULEINFO info;
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle (nullptr), &info, sizeof (info));
    return static_cast<char*> (info.lpBaseOfDll);
  }
  constexpr std::intptr_t const rebase_base(0x140000000);
}

inline void* rebase (std::size_t offset)
{
  return static_cast<void*> (module_base() + offset - rebase_base);
}
inline std::size_t unrebase (void const* pointer)
{
  return static_cast<char const*> (pointer) - module_base() + rebase_base;
}

template<typename T, bool unprotect = true>
struct var
{
	size_t const _offset;
  
  constexpr var (size_t offset) : _offset (offset) {}

	T* _x = nullptr;
	DWORD old = 0;

	operator void*() { maybe_rebase(); return _x; }
	T& operator*() { maybe_rebase();  return *_x; }
	T* operator->() { maybe_rebase(); return _x; }

	void maybe_rebase()
	{
		if (_x) return;

		_x = static_cast<T*> (rebase (_offset));

		maybe_unprotect<unprotect, T>::apply(_x, &old);
	}
	~var()
	{
		if (_x) maybe_unprotect<unprotect, T>::remove(_x, old);
	}
};

template<typename Ret, typename... Args>
struct fun<Ret(Args...)> : var<Ret(Args...), false>
{
  using signature = Ret(Args...);

  using var<Ret(Args...), false>::var;
	using var<Ret(Args...), false>::operator void *;
	Ret operator() (Args... args)
	{
		maybe_rebase();
		return (*_x)(args...);
	}
};
template<typename Ret, typename T, typename... Args>
struct fun<Ret (T::*) (Args...)> : var<Ret(T*, Args...), false>
{
  using var<Ret(T*, Args...), false>::var;
	using var<Ret(T*, Args...), false>::operator void *;
	Ret operator() (T* t, Args... args)
	{
		maybe_rebase();
		return (*_x)(t, args...);
	}
};

template<unsigned char... nth_bytes>
struct patch : public var<unsigned char[sizeof... (nth_bytes)]>
{
	unsigned char original[sizeof... (nth_bytes)];
	patch(size_t offset)
    : var (offset)
	{
		maybe_rebase();
		memcpy(original, _x, sizeof... (nth_bytes));
		unsigned char const replace[sizeof... (nth_bytes)] = { nth_bytes... };
		memcpy(_x, replace, sizeof... (nth_bytes));
	}
	~patch()
	{
		memcpy(_x, original, sizeof... (nth_bytes));
	}
};
template<typename T>
struct dynpatch<var<T, true>>
{
	T original;
	var<T, true>& _to;
	dynpatch (var<T, true>& to, T const& replace)
		: _to (to)
	{
		original = *_to;
		*_to = replace;
	}
	~dynpatch()
	{
		*_to = original;
	}
};

namespace
{
  char hexchar2char (char c)
  {
    switch (c)
    {
      case '0': return 0x0;
      case '1': return 0x1;
      case '2': return 0x2;
      case '3': return 0x3;
      case '4': return 0x4;
      case '5': return 0x5;
      case '6': return 0x6;
      case '7': return 0x7;
      case '8': return 0x8;
      case '9': return 0x9;
      case 'a': case 'A': return 0xA;
      case 'b': case 'B': return 0xB;
      case 'c': case 'C': return 0xC;
      case 'd': case 'D': return 0xD;
      case 'e': case 'E': return 0xE;
      case 'f': case 'F': return 0xF;
    default: std::cerr << "bad hex char: " << c << "\n"; abort();
    }
  }
  char hexstr2char (char const* str)
  {
    if (*str && *(str + 1))
    {
      return hexchar2char (*str) << 4 | hexchar2char (*(str + 1));
    }
    std::cerr << "bad hex string: " << str << "\n";
    abort();
  }  
}

std::intptr_t search_pattern (char const* pattern, occurence_requirement occreq, size_t after)
{
  constexpr std::intptr_t const rebase_base(0x140000000);
  MODULEINFO info;
  GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &info, sizeof(info));
  
  char const* cur_pat = pattern;
  char* ptr (static_cast<char*>(info.lpBaseOfDll) + (after ? (after - rebase_base) : 0));
  char* end (static_cast<char*>(info.lpBaseOfDll) + info.SizeOfImage);
  char* cur_addr = ptr;
  
  std::intptr_t found_at = 0;
  std::size_t occurences (0);
  
  while (ptr < end)
  {
    if ((*cur_pat == '?' && *(cur_pat + 1) == '?') || *ptr == hexstr2char (cur_pat))
    {
      cur_pat += 2; while (*cur_pat == ' ') ++cur_pat;
      if (!*cur_pat)
      {
        if (!found_at) found_at = cur_addr - static_cast<char*> (info.lpBaseOfDll) + rebase_base + 1;
        ++occurences;
		
        cur_pat = pattern;
        cur_addr = ptr;
      }
    }
    else
    {
      cur_pat = pattern;
      cur_addr = ptr;
    }
    ++ptr;
  }
  
  //std::cerr << "find pattern (" << occurences << " times): " << pattern << " -> " << std::hex << found_at << "\n";
		
  if (found_at)
  {
  	if (occreq == require_unique && occurences != 1) 
    {
	    std::cerr << "found pattern '" << pattern << "' twice!\n";
	    abort();
    }
    
	  return found_at;
  }
  
  std::cerr << "unable to find pattern " << pattern << "\n";
  abort();
}

template<typename Fun>
void hook (Fun& fun, typename Fun::signature* replacement)
{
#define FORCE(what_,...)	\
	if (FAILED (__VA_ARGS__)) {\
		std::wcerr << "Failed to " << what_ << ": " << RtlGetLastErrorString() << "\n";\
		abort();\
}

	HOOK_TRACE_INFO hHook {0};
	FORCE ("install hook", LhInstallHook(fun, replacement, nullptr, &hHook));

	ULONG ACLEntries {0};
	FORCE("set hook acl", LhSetExclusiveACL(&ACLEntries, 1, &hHook));
}

struct scoped_hook
{
  HOOK_TRACE_INFO hHook {0};
  
  template<typename Fun, typename Fun2>
    scoped_hook (Fun&& fun, Fun2&& replacement, bool exclude_this_thread = true)
  {
    FORCE ("install hook", LhInstallHook(fun, replacement, nullptr, &hHook));

    ULONG ACLEntries {0};
    FORCE("set hook acl", LhSetExclusiveACL(&ACLEntries, exclude_this_thread ? 1 : 0, &hHook));
  }
  ~scoped_hook()
  {
    FORCE ("uninstall hook", LhUninstallHook (&hHook));
  }
};