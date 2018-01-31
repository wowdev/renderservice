#pragma once

enum occurence_requirement {
	allow_multiple,
	require_unique,
};
std::intptr_t search_pattern (char const* pattern, occurence_requirement occreq = require_unique, size_t after = 0);

template<typename T, bool unprotect>
  struct var;
template<typename Fun> 
  struct fun;
template<unsigned char... nth_bytes>
  struct patch;
template<typename> 
  struct dynpatch;

template<typename Fun, typename Fun2>
  void hook (Fun&& fun, Fun2&& replacement);
struct scoped_hook;

#include "patching.ipp"