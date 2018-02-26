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

template<typename Fun>
  void hook (Fun& fun, typename Fun::signature* replacement);
struct scoped_hook;

void* rebase (size_t);
size_t unrebase (void const*);

#include "patching.ipp"